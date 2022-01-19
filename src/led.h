#ifndef LED_MANAGER
#define LED_MANAGER

#include <stdio.h>
#include <pico/stdio.h>
#include "hardware/pio.h"
#include "data_protocol.h"
#include "generated/ws2812.pio.h"

#define LED_COUNT 10
#define SECTION_COUNT 1

void ws2812_init(void);

void led_effect_update_task(unsigned int i);

void ws2812_update_task(void);

uint8_t ws2812_fill_leds(uint8_t start_led, uint8_t end_led, uint8_t value, uint8_t *data);

uint8_t ws2812_fill_section(uint8_t section_id, uint8_t value, uint8_t *data);

#endif //LED_MANAGER
