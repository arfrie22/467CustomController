#include "encoder.h"

static int16_t rotation = 0;
static int16_t encoder_min = -255;
static int16_t encoder_max = 255;

static void pio_irq_handler() {
    // test if irq 0 was raised
    if (pio0_hw->irq & 1) {
        rotation -= 1;
    }
    // test if irq 1 was raised
    if (pio0_hw->irq & 2) {
        rotation += 1;
    }
    // clear both interrupts
    pio0_hw->irq = 3;
}

void encoder_init(uint8_t rotary_encoder_A, int16_t min_value, int16_t max_value, int16_t initial_value) {
    uint8_t rotary_encoder_B = rotary_encoder_A + 1;
    // pio 0 is used
    PIO pio = pio0;
    // state machine 0
    uint8_t sm = 0;
    // configure the used pins as input with pull up
    pio_gpio_init(pio, rotary_encoder_A);
    gpio_set_pulls(rotary_encoder_A, true, false);
    pio_gpio_init(pio, rotary_encoder_B);
    gpio_set_pulls(rotary_encoder_B, true, false);
    // load the pio program into the pio memory
    uint offset = pio_add_program(pio, &pio_rotary_encoder_program);
    // make a sm config
    pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
    // set the 'in' pins
    sm_config_set_in_pins(&c, rotary_encoder_A);
    // set shift to left: bits shifted by 'in' enter at the least
    // significant bit (LSB), no autopush
    sm_config_set_in_shift(&c, false, false, 0);
    // set the IRQ handler
    irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler);
    // enable the IRQ
    irq_set_enabled(PIO0_IRQ_0, true);
    pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
    // init the sm.
    // Note: the program starts after the jump table -> initial_pc = 16
    pio_sm_init(pio, sm, 16, &c);
    // enable the sm
    pio_sm_set_enabled(pio, sm, true);

    encoder_max = max_value;
    encoder_min = min_value;
    if (initial_value > encoder_max) initial_value = encoder_max;
    if (initial_value < encoder_min) initial_value = encoder_max;
    rotation = initial_value;
}

// set the current rotation to a specific value
void encoder_set_rotation(int16_t _rotation) {
    if (_rotation > encoder_max) _rotation = encoder_max;
    if (_rotation < encoder_min) _rotation = encoder_max;
    rotation = _rotation;
}

// get the current rotation
int16_t encoder_get_rotation() {
    return rotation;
}

int16_t encoder_get_max() {
    return encoder_max;
}

int16_t encoder_get_min() {
    return encoder_min;
}

void inc_encoder() {
    if (rotation < encoder_max) rotation += 1;
}

void dec_encoder() {
    if (rotation > encoder_min) rotation -= 1;
}
