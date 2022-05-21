#include "led.h"

static const uint8_t led_gamma[] = { // Brightness ramp for LEDs
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
        2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6,
        6, 6, 6, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 11,
        11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18,
        18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27,
        28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39,
        40, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53,
        54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71,
        72, 74, 75, 76, 77, 79, 80, 81, 83, 84, 85, 87, 88, 89, 91, 92,
        94, 95, 97, 98, 99, 101, 103, 104, 106, 107, 109, 110, 112, 113, 115, 117,
        118, 120, 122, 123, 125, 127, 129, 130, 132, 134, 136, 138, 139, 141, 143, 145,
        147, 149, 151, 153, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177,
        179, 182, 184, 186, 188, 190, 193, 195, 197, 200, 202, 204, 207, 209, 211, 214,
        216, 219, 221, 223, 226, 228, 231, 234, 236, 239, 241, 244, 247, 249, 252, 255};

// WS2812

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio1, 0, pixel_grb << 8u);
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
    if (t < (float) 1 / 6) return p + (q - p) * 6 * t;
    if (t < (float) 1 / 2) return q;
    if (t < (float) 2 / 3) return p + (q - p) * ((float) 2 / 3 - t) * 6;
    return p;
}

static uint32_t hslToRGB(float h, float s, float l) {
    float r, g, b;

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        float q = (l < 0.5) ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = hueToRGB(p, q, h + (float) 1 / 3);
        g = hueToRGB(p, q, h);
        b = hueToRGB(p, q, h - (float) 1 / 3);
    }

    return
            ((uint32_t) (r * 255) << 16) |
            ((uint32_t) (g * 255) << 8) |
            (uint32_t) (b * 255);
}


uint8_t LED_RGB_BUFFER[LED_COUNT * 3] = {0x00};
uint8_t LED_RGB_OUTPUT_BUFFER[LED_COUNT * 3] = {0x00};
uint8_t LED_EFFECT_BUFFER[LED_COUNT * 3] = {0x00};
uint8_t LED_BRIGHTNESS_BUFFER[LED_COUNT] = {0xFF};
uint8_t SECTION_BUFFER[SECTION_COUNT * 2] = {0, 9, 10, 14, 15, 24, 25, 29, 30, 35, 36, 41, 30, 41, 0, 29};


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

//--------------------------------------------------------------------+
// WS2812 UPDATE TASK
//--------------------------------------------------------------------+
void ws2812_init(void)
{
    //set_sys_clock_48();
    stdio_init_all();
    puts("WS2812 Smoke Test");

    // todo get free sm
    PIO pio = pio1;
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
