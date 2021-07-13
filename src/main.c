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
#include <pico/bootrom.h>
#include <math.h>

#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "generated/ws2812.pio.h"
#include "data_protocol.h"

/* This example demonstrate HID Generic raw Input & Output.
 * It will receive data from Host (In endpoint) and echo back (Out endpoint).
 * HID Report descriptor use vendor for usage page (using template TUD_HID_REPORT_DESC_GENERIC_INOUT)
 *
 * There are 2 ways to test the sketch
 * 1. Using nodejs
 *    - Install nodejs and npm to your PC
 *    - Install excellent node-hid (https://github.com/node-hid/node-hid) by
 *      $ npm install node-hid
 *    - Run provided hid test script
 *      $ node hid_test.js
 *
 * 2. Using python hidRun
 *    - Python and `hid` package is required, for installation please follow https://pypi.org/project/hid/
 *    - Run provided hid test script to send and receive data to this device.
 *      $ python3 hid_test.py
 */

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

enum {
    ITF_CONTROLLER = 0,
    ITF_DATA = 1
};

static const uint8_t led_gamma[] = { // Brightness ramp for LEDs
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,
2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,
6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 11,
11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18,
18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27,
28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39,
40, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53,
54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71,
72, 74, 75, 76, 77, 79, 80, 81, 83, 84, 85, 87, 88, 89, 91, 92,
94, 95, 97, 98, 99,101,103,104,106,107,109,110,112,113,115,117,
118,120,122,123,125,127,129,130,132,134,136,138,139,141,143,145,
147,149,151,153,155,157,159,161,163,165,167,169,171,173,175,177,
179,182,184,186,188,190,193,195,197,200,202,204,207,209,211,214,
216,219,221,223,226,228,231,234,236,239,241,244,247,249,252,255 };

// WS2812

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

static float hueToRGB(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < (float) 1/6) return p + (q - p) * 6 * t;
    if (t < (float) 1/2) return q;
    if (t < (float) 2/3) return p + (q - p) * ((float) 2/3 - t) * 6;
    return p;
}

static uint32_t hslToRGB(float h, float s, float l) {
    float r, g, b;

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        float q = (l < 0.5) ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = hueToRGB(p, q, h + (float) 1/3);
        g = hueToRGB(p, q, h);
        b = hueToRGB(p, q, h - (float) 1/3);
    }

    return
            ((uint32_t) (r * 255) << 16) |
            ((uint32_t) (g * 255) << 8) |
            (uint32_t) (b * 255);
}


#define PIN_TX 0
#define LED_COUNT 10
#define SECTION_COUNT 1

uint8_t LED_RGB_BUFFER[LED_COUNT * 3] = {0x00};
uint8_t LED_RGB_OUTPUT_BUFFER[LED_COUNT * 3] = {0x00};
uint8_t LED_EFFECT_BUFFER[LED_COUNT * 3] = {0x00};
uint8_t LED_BRIGHTNESS_BUFFER[LED_COUNT] = {0xFF};
uint8_t SECTION_BUFFER[SECTION_COUNT * 2] = {0x00, 0x07};

void ws2812_init(void);
void led_effect_update_task(unsigned int i);
void ws2812_update_task(void);


// Effects

void effect_static(uint led, uint t) {
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 0] = LED_RGB_BUFFER[(led * 3) + 0];
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 1] = LED_RGB_BUFFER[(led * 3) + 1];
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 2] = LED_RGB_BUFFER[(led * 3) + 2];

    if (!(t % LED_EFFECT_BUFFER[(led * 3) + 2]))
        LED_EFFECT_BUFFER[(led * 3) + 1] += 1;
}

void effect_breathing_up(uint led, uint t) {
    uint r, g, b;
    uint8_t multiplier = LED_EFFECT_BUFFER[(led * 3) + 1];

    r = LED_RGB_BUFFER[(led * 3) + 0];
    g = LED_RGB_BUFFER[(led * 3) + 1];
    b = LED_RGB_BUFFER[(led * 3) + 2];

    r *= multiplier;
    g *= multiplier;
    b *= multiplier;

    r /= 0xFF;
    g /= 0xFF;
    b /= 0xFF;

    LED_RGB_OUTPUT_BUFFER[(led * 3) + 0] = (uint8_t) r;
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 1] = (uint8_t) g;
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 2] = (uint8_t) b;

    if (!(t % LED_EFFECT_BUFFER[(led * 3) + 2])) {
        if (LED_EFFECT_BUFFER[(led * 3) + 1] == 0xFF)
            LED_EFFECT_BUFFER[(led * 3) + 0] += 1;

        LED_EFFECT_BUFFER[(led * 3) + 1] += 1;
    }
}

void effect_breathing_down(uint led, uint t) {
    uint r, g, b;
    uint8_t multiplier = 0xFF - LED_EFFECT_BUFFER[(led * 3) + 1];

    r = LED_RGB_BUFFER[(led * 3) + 0];
    g = LED_RGB_BUFFER[(led * 3) + 1];
    b = LED_RGB_BUFFER[(led * 3) + 2];

    r *= multiplier;
    g *= multiplier;
    b *= multiplier;

    r /= 0xFF;
    g /= 0xFF;
    b /= 0xFF;

    LED_RGB_OUTPUT_BUFFER[(led * 3) + 0] = (uint8_t) r;
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 1] = (uint8_t) g;
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 2] = (uint8_t) b;

    if (!(t % LED_EFFECT_BUFFER[(led * 3) + 2])) {
        if (LED_EFFECT_BUFFER[(led * 3) + 1] == 0xFF)
            LED_EFFECT_BUFFER[(led * 3) + 0] -= 1;

        LED_EFFECT_BUFFER[(led * 3) + 1] += 1;
    }
}

void effect_color_cycle(uint led, uint t) {
    float h = (float) (LED_EFFECT_BUFFER[(led * 3) + 1] % 256) / 255;
    uint32_t color = hslToRGB(h, 1, 0.5);
    uint8_t r = ((color >> 16) & 0xff);
    uint8_t g = ((color >> 8) & 0xff);
    uint8_t b = ((color) & 0xff);

    LED_RGB_OUTPUT_BUFFER[(led * 3) + 0] = (uint8_t) r;
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 1] = (uint8_t) g;
    LED_RGB_OUTPUT_BUFFER[(led * 3) + 2] = (uint8_t) b;

    if (!(t % LED_EFFECT_BUFFER[(led * 3) + 2]))
        LED_EFFECT_BUFFER[(led * 3) + 1] += 1;
}


typedef void (*effect)(uint led, uint t);
const struct {
    effect eff;
} effect_table[] = {
        {effect_static},
        {effect_breathing_up},
        {effect_breathing_down},
        {effect_color_cycle},
};

uint8_t ws2812_fill_leds(uint8_t start_led, uint8_t end_led, uint8_t value, uint8_t *data) {
    for (int i = start_led; i <= end_led; ++i) {
        switch (value) {
            case id_led_base_color: {
                LED_RGB_BUFFER[(i * 3) + 0] = data[0];
                LED_RGB_BUFFER[(i * 3) + 1] = data[1];
                LED_RGB_BUFFER[(i * 3) + 2] = data[2];
                break;
            }

            case id_led_effect: {
                LED_EFFECT_BUFFER[(i * 3) + 0] = data[0];
                LED_EFFECT_BUFFER[(i * 3) + 1] = 0x00;
                break;
            }

            case id_led_effect_spaced: {
                LED_EFFECT_BUFFER[(i * 3) + 0] = data[0];
                uint offset = ((i - start_led) * 0xFF);
                offset /= (end_led - start_led) + 1;
                LED_EFFECT_BUFFER[(i * 3) + 1] = (uint8_t) offset;
                break;
            }

            case id_led_offset: {
                LED_EFFECT_BUFFER[(i * 3) + 1] = data[0];
                break;
            }

            case id_led_speed: {
                LED_EFFECT_BUFFER[(i * 3) + 2] = data[0];
                break;
            }

            case id_led_brightness: {
                LED_BRIGHTNESS_BUFFER[i] = data[0];
                break;
            }

            default: {
                return 0;
            }
        }
    }
    return 1;
}

uint8_t ws2812_fill_section(uint8_t section_id, uint8_t value, uint8_t *data) {
    return ws2812_fill_leds(SECTION_BUFFER[(section_id * 2) + 0], SECTION_BUFFER[(section_id * 2) + 1], value, data);
}


/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
    board_init();
    tusb_init();
    ws2812_init();

    unsigned short l = 0;
    unsigned int t = 0;
    while (1)
    {
        tud_task(); // tinyusb device task
        led_blinking_task();

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
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void hid_task(void)
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if ( tud_suspended() && btn )
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }

    /*------------- Controller -------------*/
    if ( tud_hid_n_ready(ITF_CONTROLLER) )
    {
        // use to avoid send multiple consecutive zero report for keyboard
        static bool has_gamepad_key = false;

        hid_gamepad_report_t report =
                {
                        .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
                        .hat = 0, .buttons = 0
                };

        if ( btn )
        {
            report.hat = GAMEPAD_HAT_UP;
            report.buttons = GAMEPAD_BUTTON_A;
            tud_hid_report(ITF_CONTROLLER, &report, sizeof(report));

            has_gamepad_key = true;
        }else
        {
            report.hat = GAMEPAD_HAT_CENTERED;
            report.buttons = 0;
            if (has_gamepad_key) tud_hid_report(ITF_CONTROLLER, &report, sizeof(report));
            has_gamepad_key = false;
        }
    }
}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;

    // Command Handler
    if (itf == ITF_DATA) {
        uint8_t *command_id   = &(buffer[0]);
        uint8_t *command_data = &(buffer[1]);

        switch (*command_id) {
            case id_get_protocol_version: {
                command_data[0] = COMMAND_PROTOCOL_VERSION >> 8;
                command_data[1] = COMMAND_PROTOCOL_VERSION & 0xFF;
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
                        break;
                    }

                    case id_section_count: {
                        command_data[1] = SECTION_COUNT;
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
                        if (!ws2812_fill_leds(command_data[1], command_data[1], command_data[2], &(command_data[3])))
                            *command_id = id_error;
                        break;
                    }

                    case id_multiple: {
                        if (!ws2812_fill_leds(command_data[1], command_data[2], command_data[3], &(command_data[4])))
                            *command_id = id_error;
                        break;
                    }


                    case id_section: {
                        if (!ws2812_fill_section(command_data[1], command_data[2], &(command_data[3])))
                            *command_id = id_error;
                        break;
                    }

                    case id_all: {
                        if (!ws2812_fill_leds(0, LED_COUNT-1, command_data[1], &(command_data[2])))
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

            case id_enter_bootloader: {
                tud_hid_n_report(ITF_DATA, report_id, buffer, bufsize);
                reset_usb_boot(0, 0);
                break;
            }

            default: {
                *command_id = id_error;
                break;
            }
        }

        tud_hid_n_report(ITF_DATA, report_id, buffer, bufsize);
    }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // Blink every interval ms
    if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// WS2812 UPDATE TASK
//--------------------------------------------------------------------+
void ws2812_init(void)
{
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 Smoke Test");

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, PIN_TX, 800000, false);

    for (int i = 0; i < LED_COUNT; ++i) {
        LED_RGB_BUFFER[(i * 3) + 0] = 0x00;
        LED_RGB_BUFFER[(i * 3) + 1] = 0x00;
        LED_RGB_BUFFER[(i * 3) + 2] = 0x00;

        LED_RGB_OUTPUT_BUFFER[(i * 3) + 0] = 0x00;
        LED_RGB_OUTPUT_BUFFER[(i * 3) + 1] = 0x00;
        LED_RGB_OUTPUT_BUFFER[(i * 3) + 2] = 0x00;

        LED_EFFECT_BUFFER[(i * 3) + 0] = 0x00;
        LED_EFFECT_BUFFER[(i * 3) + 1] = 0x00;
        LED_EFFECT_BUFFER[(i * 3) + 2] = 0x00;

        LED_BRIGHTNESS_BUFFER[i] = 0xFF;
    }
}

void led_effect_update_task(unsigned int t)
{
    for (int i = 0; i < LED_COUNT; ++i) {
        effect_table[LED_EFFECT_BUFFER[(i * 3) + 0]].eff(i, t);
    }
}

void ws2812_update_task(void)
{
    for (int i = 0; i < LED_COUNT; ++i) {
        unsigned int r = LED_RGB_OUTPUT_BUFFER[(i * 3) + 0];
        unsigned int g = LED_RGB_OUTPUT_BUFFER[(i * 3) + 1];
        unsigned int b = LED_RGB_OUTPUT_BUFFER[(i * 3) + 2];

        r *= LED_BRIGHTNESS_BUFFER[i];
        g *= LED_BRIGHTNESS_BUFFER[i];
        b *= LED_BRIGHTNESS_BUFFER[i];

        r /= 0xFF;
        g /= 0xFF;
        b /= 0xFF;

        put_pixel(urgb_u32(r, g, b));
    }
}
