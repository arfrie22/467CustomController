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
 */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

const char** get_string_desc();

enum {
    REPORT_ID_GAMEPAD = 1,
    REPORT_ID_COUNT
};

#define HID_USAGE_PAGE_CONST 0x05
#define HID_USAGE_CONST 0x09
#define HID_COLLECTION_CONST 0xA1
#define HID_REPORT_ID_CONST 0x85
#define HID_USAGE_MIN_CONST 0x19
#define HID_USAGE_MAX_CONST 0x29
#define HID_LOGICAL_MIN_CONST 0x15
#define HID_LOGICAL_MAX_CONST 0x25
#define HID_PHYSICAL_MIN_CONST 0x35
#define HID_PHYSICAL_MAX_CONST 0x45
#define HID_REPORT_SIZE_CONST 0x75
#define HID_REPORT_COUNT_CONST 0x95
#define HID_UNIT_EXPONENT_CONST 0x55
#define HID_UNIT_CONST 0x65
#define HID_INPUT_CONST 0x81

#define HID_ANGULAR_POSITION 0x14

#define HID_USAGE_SIMULATE_RUDDER 0xBA
#define HID_USAGE_SIMULATE_THROTTLE 0xBB
#define HID_USAGE_SIMULATE_ACCELERATE 0xC4
#define HID_USAGE_SIMULATE_BRAKE 0xC5
#define HID_USAGE_SIMULATE_STEERING 0xC8

#endif /* USB_DESCRIPTORS_H_ */
