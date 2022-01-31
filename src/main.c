/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pico/bootrom.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "led.h"
#include "data_protocol.h"
#include "config.h"
#include "encoder.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);

void cdc_task(void);

void hid_task(void);

/*------------- MAIN -------------*/
int main(void) {
    board_init();
    tusb_init();
    ws2812_init();
    encoder_init(20);
    encoder_set_rotation(0);

    unsigned short l = 0;
    unsigned int t = 0;
    while (1) {
        tud_task(); // tinyusb device task
        led_blinking_task();

        cdc_task();
        hid_task();

        if (++l == 512) {
            led_effect_update_task(t);
            ws2812_update_task();
            l = 0;
            t++;
        }
    }

    return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void) {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    // if ( tud_cdc_connected() )
    {
        // connected and there are data available
        if (tud_cdc_available()) {
            // read datas
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            (void) count;

            uint8_t *command_id = &(buf[0]);
            uint8_t *command_data = &(buf[1]);

            switch (*command_id) {
                case id_get_protocol_version: {
                    command_data[0] = COMMAND_PROTOCOL_VERSION >> 8;
                    command_data[1] = COMMAND_PROTOCOL_VERSION & 0xFF;
                    count += 2;
                    break;
                }

                case id_get_controller_state: {
                    //TODO Imp
                    break;
                }

                case id_get_led_data: {
                    switch (command_data[0]) {
                        case id_led_count: {
                            command_data[1] = LED_COUNT;
                            count += 1;
                            break;
                        }

                        case id_section_count: {
                            command_data[1] = SECTION_COUNT;
                            count += 1;
                            break;
                        }

                        default: {
                            *command_id = id_error;
                            break;
                        }
                    }
                    break;
                }

                case id_get_led: {
                    //TODO Imp
                    break;
                }

                case id_set_led: {
                    switch (command_data[0]) {
                        case id_single: {
                            if (!ws2812_fill_leds(command_data[1], command_data[1], command_data[2],
                                                  &(command_data[3])))
                                *command_id = id_error;
                            break;
                        }

                        case id_multiple: {
                            if (!ws2812_fill_leds(command_data[1], command_data[2], command_data[3],
                                                  &(command_data[4])))
                                *command_id = id_error;
                            break;
                        }


                        case id_section: {
                            if (!ws2812_fill_section(command_data[1], command_data[2], &(command_data[3])))
                                *command_id = id_error;
                            break;
                        }

                        case id_all: {
                            if (!ws2812_fill_leds(0, LED_COUNT - 1, command_data[1], &(command_data[2])))
                                *command_id = id_error;
                            break;
                        }

                        default: {
                            *command_id = id_error;
                            break;
                        }
                    }
                    break;
                }

                case id_get_team_number: {
                    command_data[0] = (TEAM_NUMBER >> 24) & 0xFF;
                    command_data[1] = (TEAM_NUMBER >> 16) & 0xFF;
                    command_data[2] = (TEAM_NUMBER >> 8) & 0xFF;
                    command_data[3] = TEAM_NUMBER & 0xFF;
                    count += 4;
                    break;
                }

                case id_enter_bootloader: {
                    tud_cdc_write(buf, sizeof(buf));
                    tud_cdc_write_flush();
                    reset_usb_boot(0, 0);
                    break;
                }

                default: {
                    *command_id = id_error;
                    break;
                }
            }

            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
    }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    (void) itf;
    (void) rts;

    // TODO set some indicator
    if (dtr) {
        // Terminal connected
    } else {
        // Terminal disconnected
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
    (void) itf;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // skip if hid is not ready yet
        if (!tud_hid_ready()) {
            return;
        }

        hid_controller_report_t report =
                {
                        .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
                        .hats = 0, .buttons = 0
                };

        uint8_t hat1 = 8;
        uint8_t hat2 = 8;

        if (btn) {
            hat1 = GAMEPAD_HAT_UP;
            hat2 = GAMEPAD_HAT_UP_RIGHT;
            report.buttons = 0xFFFFFFFF;
            inc_encoder(); // TODO yolo
            report.x = encoder_16_bit();

        } else {
            report.buttons = 0;
            dec_encoder();
            report.x = encoder_16_bit();
        }

        report.hats = hat1 << 4 | (hat2 & 0x0F);

        tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len) {
    (void) instance;
    (void) len;

    uint8_t next_report_id = report[0] + 1;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
    // TODO not Implemented
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
    (void) instance;

}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // blink is disabled
    if (!blink_interval_ms) return;

    // Blink every interval ms
    if (board_millis() - start_ms < blink_interval_ms) return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}
