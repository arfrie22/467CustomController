#include "input.h"

#define BUTTON_1_PIN 0
#define BUTTON_2_PIN 0
#define BUTTON_3_PIN 0
#define BUTTON_4_PIN 0
#define BUTTON_5_PIN 0
#define BUTTON_6_PIN 0
#define BUTTON_7_PIN 0


static long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static int16_t min(short a, short b) {
    return b >= a ? a : b;
}

static int16_t max(int16_t a, int16_t b) {
    return a < b ? b : a;
}

int16_t encode_16_bit_value(int16_t value, int16_t valueMinimum, int16_t valueMaximum, int16_t actualMinimum, int16_t actualMaximum) {
    int16_t convertedValue;
    int16_t realMinimum = min(valueMinimum, valueMaximum);
    int16_t realMaximum = max(valueMinimum, valueMaximum);

    if (value < realMinimum) {
        value = realMinimum;
    }
    if (value > realMaximum) {
        value = realMaximum;
    }

    if (valueMinimum > valueMaximum) {
        // Values go from a larger number to a smaller number (e.g. 1024 to 0)
        value = realMaximum - value + realMinimum;
    }

    convertedValue = map(value, realMinimum, realMaximum, actualMinimum, actualMaximum);

    return convertedValue;
}


bool get_button(uint8_t button);
uint8_t get_hat_1();
uint8_t get_hat_2();
uint16_t get_x_axis();
uint16_t get_y_axis();
uint16_t get_z_axis();
uint16_t get_rx_axis();
uint16_t get_ry_axis();
uint16_t get_rz_axis();
uint16_t get_rudder();
uint16_t get_throttle();
uint16_t get_accelerator();
uint16_t get_brake();
uint16_t get_steering();

void update_report(uint8_t *report) {
    uint8_t report_index = 0;
#if BUTTON_COUNT
    for (int i = 0; i < BUTTON_COUNT; ++i) {
        if (!(i % 8) && (i > 0)) report_index += 1;
        report[report_index] |= get_button(i) << (i % 8);
    }
#endif //BUTTON_COUNT

#if HAT_COUNT
    report_index += 1;
    report[report_index] |= (get_hat_1() << 4);
#if HAT_COUNT > 1
    report[report_index] |= (get_hat_2() & 0x0F);
#endif //HAT_COUNT > 1
#endif //HAT_COUNT

#if HAS_X_AXIS
    report_index += 1;
    report[report_index] = get_x_axis() & 0xFF;
    report_index += 1;
    report[report_index] = get_x_axis() >> 8;
#endif //HAS_X_AXIS
#if HAS_Y_AXIS
    report_index += 1;
    report[report_index] = get_y_axis() & 0xFF;
    report_index += 1;
    report[report_index] = get_y_axis() >> 8;
#endif //HAS_Y_AXIS
#if HAS_Z_AXIS
    report_index += 1;
    report[report_index] = get_z_axis() & 0xFF;
    report_index += 1;
    report[report_index] = get_z_axis() >> 8;
#endif //HAS_Z_AXIS
#if HAS_RX_AXIS
    report_index += 1;
    report[report_index] = get_rx_axis() & 0xFF;
    report_index += 1;
    report[report_index] = get_rx_axis() >> 8;
#endif //HAS_RX_AXIS
#if HAS_RY_AXIS
    report_index += 1;
    report[report_index] = get_ry_axis() & 0xFF;
    report_index += 1;
    report[report_index] = get_ry_axis() >> 8;
#endif //HAS_RY_AXIS
#if HAS_RZ_AXIS
    report_index += 1;
    report[report_index] = get_rz_axis() & 0xFF;
    report_index += 1;
    report[report_index] = get_rz_axis() >> 8;
#endif //HAS_RZ_AXIS

#if HAS_RUDDER
    report_index += 1;
    report[report_index] = get_rudder() & 0xFF;
    report_index += 1;
    report[report_index] = get_rudder() >> 8;
#endif //HAS_RUDDER
#if HAS_THROTTLE
    report_index += 1;
    report[report_index] = get_throttle() & 0xFF;
    report_index += 1;
    report[report_index] = get_throttle() >> 8;
#endif //HAS_THROTTLE
#if HAS_ACCELERATOR
    report_index += 1;
    report[report_index] = get_accelerator() & 0xFF;
    report_index += 1;
    report[report_index] = get_accelerator() >> 8;
#endif //HAS_ACCELERATOR
#if HAS_BRAKE
    report_index += 1;
    report[report_index] = get_brake() & 0xFF;
    report_index += 1;
    report[report_index] = get_brake() >> 8;
#endif //HAS_BRAKE
#if HAS_STEERING
    report_index += 1;
    report[report_index] = get_steering() & 0xFF;
    report_index += 1;
    report[report_index] = get_steering() >> 8;
#endif //HAS_STEERING
}

static inline void init_pin(uint8_t pin) {
    gpio_set_dir(pin, false);
    gpio_set_pulls(pin, true, false);
}

void input_init() {
    init_pin(1);
    init_pin(2);
    init_pin(3);
    init_pin(4);
    init_pin(5);
    init_pin(6);
    init_pin(7);
    init_pin(8);
    init_pin(9);
    init_pin(10);
    init_pin(11);
    init_pin(12);

    // encoder_init(20, -255, 255, 0);
}

static inline bool pin_get(uint8_t pin) {
    return !gpio_get(pin);
}

bool get_button(uint8_t button) {
    switch (button) {
        case 0:
            return pin_get(1);
        case 1:
            return pin_get(2);
        case 2:
            return pin_get(3);
        case 3:
            return pin_get(4);
        case 4:
            return pin_get(5);
        case 5:
            return pin_get(6);
        case 6:
            return pin_get(7);
        case 7:
            return pin_get(8);
        case 8:
            return pin_get(9);
        case 9:
            return pin_get(10);
        case 10:
            return pin_get(11);
        case 11:
            return pin_get(12);
        default:
            return 0;
    }
}

uint8_t get_hat_1() {
    return pin_get(1)*8;
}

uint8_t get_hat_2() {
    return pin_get(2)*8;
}

uint16_t get_x_axis() {
    return encode_16_bit_value(encoder_get_rotation(), encoder_get_min(), encoder_get_max(), -32767, 32767);
}

uint16_t get_y_axis() {
    return encode_16_bit_value(4, 0, 8, -32767, 32767);
}

uint16_t get_z_axis() {
    return encode_16_bit_value(-1, -1, 1, -32767, 32767);
}

uint16_t get_rx_axis() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_ry_axis() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_rz_axis() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_rudder() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_throttle() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_accelerator() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_brake() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}

uint16_t get_steering() {
    return encode_16_bit_value(0, -1, 1, -32767, 32767);
}
