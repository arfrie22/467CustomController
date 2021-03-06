
#ifndef INPUT
#define INPUT

#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include <hardware/gpio.h>
#include "encoder.h"

#ifndef BUTTON_COUNT
#define BUTTON_COUNT 0
#endif //BUTTON_COUNT

#if BUTTON_COUNT % 8
#define BUTTON_PADDING (8 - (BUTTON_COUNT % 8))
#else
#define BUTTON_PADDING 0
#endif //BUTTON_COUNT % 8

#define HAS_BUTTON_PADDING (BUTTON_PADDING > 0)

#ifndef HAT_COUNT
#define HAT_COUNT 0
#endif //HAT_COUNT

#ifndef HAS_X_AXIS
#define HAS_X_AXIS false
#endif //HAS_X_AXIS

#ifndef HAS_Y_AXIS
#define HAS_Y_AXIS false
#endif //HAS_Y_AXIS

#ifndef HAS_Z_AXIS
#define HAS_Z_AXIS false
#endif //HAS_Z_AXIS

#ifndef HAS_RX_AXIS
#define HAS_RX_AXIS false
#endif //HAS_RX_AXIS

#ifndef HAS_RY_AXIS
#define HAS_RY_AXIS false
#endif //HAS_RY_AXIS

#ifndef HAS_RZ_AXIS
#define HAS_RZ_AXIS false
#endif //HAS_RZ_AXIS

#define AXIS_COUNT ((HAS_X_AXIS == true) + (HAS_Y_AXIS == true) + (HAS_Z_AXIS == true) + (HAS_RX_AXIS == true) + (HAS_RY_AXIS == true) + (HAS_RZ_AXIS == true))


#ifndef HAS_RUDDER
#define HAS_RUDDER false
#endif //HAS_RUDDER

#ifndef HAS_THROTTLE
#define HAS_THROTTLE false
#endif //HAS_THROTTLE

#ifndef HAS_ACCELERATOR
#define HAS_ACCELERATOR false
#endif //HAS_ACCELERATOR

#ifndef HAS_BRAKE
#define HAS_BRAKE false
#endif //HAS_BRAKE

#ifndef HAS_STEERING
#define HAS_STEERING false
#endif //HAS_STEERING

#define SIMULATION_COUNT ((HAS_RUDDER == true) + (HAS_THROTTLE == true) + (HAS_ACCELERATOR == true) + (HAS_BRAKE == true) + (HAS_STEERING == true))



#define HID_DESC_START_BYTES 8

#if HAS_BUTTON_PADDING
#define HID_DESC_BUTTON_PADDING_BYTES 6
#else
#define HID_DESC_BUTTON_PADDING_BYTES 0
#endif //HAS_BUTTON_PADDING

#if BUTTON_COUNT
#define HID_DESC_BUTTON_BYTES 20 + HID_DESC_BUTTON_PADDING_BYTES
#else
#define HID_DESC_BUTTON_BYTES 0
#endif //BUTTON_COUNT

#if HAT_COUNT | AXIS_COUNT
#define HID_DESC_DESKTOP_BYTES 2
#else
#define HID_DESC_DESKTOP_BYTES 0
#endif //HAT_COUNT | AXIS_COUNT

#if HAT_COUNT
#if HAT_COUNT > 1
#define HID_DESC_DESC_HAT_BYTES 19 + 19
#else
#define HID_DESC_DESC_HAT_BYTES 19 + 6
#endif //HAT_COUNT > 1
#else
#define HID_DESC_DESC_HAT_BYTES 0
#endif //HAT_COUNT

#if AXIS_COUNT
#define HID_DESC_AXIS_BYTES 14 + (AXIS_COUNT * 2) + 3
#else
#define HID_DESC_AXIS_BYTES 0
#endif //AXIS_COUNT

#if SIMULATION_COUNT
#define HID_DESC_SIMULATION_BYTES 14 + (SIMULATION_COUNT * 2) + 3
#else
#define HID_DESC_SIMULATION_BYTES 0
#endif //SIMULATION_COUNT

#define HID_DESC_END_BYTES 1

#define HID_REPORT_DESC_LENGTH HID_DESC_START_BYTES + HID_DESC_BUTTON_BYTES + HID_DESC_DESKTOP_BYTES + HID_DESC_DESC_HAT_BYTES + HID_DESC_AXIS_BYTES + HID_DESC_SIMULATION_BYTES + HID_DESC_END_BYTES

#define HID_REPORT_LENGTH (BUTTON_COUNT / 8) + ((HAS_BUTTON_PADDING) == true) + (HAT_COUNT > 0) + (AXIS_COUNT * 2) + (SIMULATION_COUNT * 2)
#define HAT_REPORT_INDEX ((BUTTON_COUNT + BUTTON_PADDING)/8)


void input_init();
void update_report(uint8_t *report);
#endif //INPUT
