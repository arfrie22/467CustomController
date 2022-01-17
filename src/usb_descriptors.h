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

enum
{
  REPORT_ID_GAMEPAD = 1,
  REPORT_ID_COUNT
};

// Gamepad Report Descriptor Template
// with 16 buttons, 2 joysticks and 1 hat/dpad with following layout
// | X | Y | Z | Rz | Rx | Ry (1 byte each) | hat/DPAD (1 byte) | Button Map (2 bytes) |
#define TUD_HID_REPORT_DESC_CONTROLLER(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                 ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_JOYSTICK )                 ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                 ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    /* 16 bit Button Map */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_BUTTON                  ) ,\
    HID_USAGE_MIN    ( 1                                      ) ,\
    HID_USAGE_MAX    ( 32                                     ) ,\
    HID_LOGICAL_MIN  ( 0                                      ) ,\
    HID_LOGICAL_MAX  ( 1                                      ) ,\
    HID_REPORT_SIZE  ( 1                                      ) ,\
    HID_REPORT_COUNT ( 32                                     ) ,\
    HID_UNIT_EXPONENT ( 0                                     ) ,\
    HID_UNIT         ( 0x00                                   ) ,\
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 2, 4 bit DPad/Hat Button Map  */ \
    /* Hat 1 */ \
    HID_USAGE_PAGE   ( HID_USAGE_PAGE_DESKTOP                 ) ,\
    HID_USAGE        ( HID_USAGE_DESKTOP_HAT_SWITCH           ) ,\
    HID_LOGICAL_MIN  ( 0                                      ) ,\
    HID_LOGICAL_MAX  ( 7                                      ) ,\
    HID_PHYSICAL_MIN ( 0                                      ) ,\
    HID_PHYSICAL_MAX_N ( 315, 2                               ) ,\
    HID_UNIT         ( 0x14                                   ) ,\
    HID_REPORT_SIZE  ( 4                                      ) ,\
    HID_REPORT_COUNT ( 1                                      ) ,\
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* Hat 2 */ \
    HID_USAGE        ( HID_USAGE_DESKTOP_HAT_SWITCH           ) ,\
    HID_LOGICAL_MIN  ( 0                                      ) ,\
    HID_LOGICAL_MAX  ( 7                                      ) ,\
    HID_PHYSICAL_MIN ( 0                                      ) ,\
    HID_PHYSICAL_MAX_N ( 315, 2                               ) ,\
    HID_UNIT         ( 0x14                                   ) ,\
    HID_REPORT_SIZE  ( 4                                      ) ,\
    HID_REPORT_COUNT ( 1                                      ) ,\
    HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 16 bit X, Y, Z, Rz, Rx, Ry (min -127, max 127 ) */ \
    HID_USAGE        ( HID_USAGE_DESKTOP_POINTER              ) ,\
    HID_LOGICAL_MIN_N ( -32767, 2                             ) ,\
    HID_LOGICAL_MAX_N (  32767, 2                               ) ,\
    HID_REPORT_SIZE  ( 16                                     ) ,\
    HID_REPORT_COUNT ( 6                                      ) ,\
    HID_COLLECTION   ( HID_COLLECTION_PHYSICAL                ) ,\
        HID_USAGE        ( HID_USAGE_DESKTOP_X                    ) ,\
        HID_USAGE        ( HID_USAGE_DESKTOP_Y                    ) ,\
        HID_USAGE        ( HID_USAGE_DESKTOP_Z                    ) ,\
        HID_USAGE        ( HID_USAGE_DESKTOP_RX                   ) ,\
        HID_USAGE        ( HID_USAGE_DESKTOP_RY                   ) ,\
        HID_USAGE        ( HID_USAGE_DESKTOP_RZ                   ) ,\
        HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    HID_COLLECTION_END                                          ,\
  HID_COLLECTION_END \

typedef struct TU_ATTR_PACKED
{
    uint32_t buttons;  ///< Buttons mask for currently pressed buttons
    uint8_t hats;       ///< Buttons mask for currently pressed buttons in the DPad/hat
    int16_t  x;         ///< Delta x  movement of left analog-stick
    int16_t  y;         ///< Delta y  movement of left analog-stick
    int16_t  z;         ///< Delta z  movement of right analog-joystick
    int16_t  rz;        ///< Delta Rz movement of right analog-joystick
    int16_t  rx;        ///< Delta Rx movement of analog left trigger
    int16_t  ry;        ///< Delta Ry movement of analog right trigger
}hid_controller_report_t;

#endif /* USB_DESCRIPTORS_H_ */
