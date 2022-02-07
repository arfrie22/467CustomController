
#ifndef ENCODER
#define ENCODER

#include <stdio.h>
#include <pico/stdio.h>
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "data_protocol.h"
#include "generated/pio_rotary_encoder.pio.h"
#include <math.h>

struct encoder {
    int position;
};

typedef struct encoder encoder;

void encoder_init(uint8_t rotary_encoder_A, int16_t min_value, int16_t max_value, int16_t initial_value);
void encoder_set_rotation(int16_t _rotation);
int16_t encoder_get_rotation();
int16_t encoder_get_max();
int16_t encoder_get_min();
void inc_encoder();
void dec_encoder();

#endif //ENCODER
