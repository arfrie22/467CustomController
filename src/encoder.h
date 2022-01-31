
#ifndef ENCODER
#define ENCODER

#include <stdio.h>
#include <pico/stdio.h>
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "data_protocol.h"
#include "generated/pio_rotary_encoder.pio.h."
#include <math.h>

struct encoder {
    int position;
};

typedef struct encoder encoder;

void encoder_init(uint8_t rotary_encoder_A);
void encoder_set_rotation(int16_t _rotation);
int16_t encoder_get_rotation();
void inc_encoder();
void dec_encoder();
int16_t encoder_16_bit();

#endif //ENCODER
