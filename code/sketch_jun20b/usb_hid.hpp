#ifndef GUARD_USB_HID_HPP
#define GUARD_USB_HID_HPP

#include <Adafruit_TinyUSB.h>

#include "config.hpp"

extern uint8_t const desc_hid_report_keyboard[]; //= {TUD_HID_REPORT_DESC_KEYBOARD()};
extern uint8_t const desc_hid_report_controller[]; //= {TUD_HID_REPORT_DESC_GAMEPAD()};

// HID STUFF :
extern Adafruit_USBD_HID usb_keyboard;
extern Adafruit_USBD_HID usb_controller;
//Adafruit_USBD_HID usb_mouse;

// Gamepad stuff : 
extern hid_gamepad_report_t gamepad;

void setup_usb();

void send_usb_report( Adafruit_USBD_HID* hid, hid_gamepad_report_t * report);

void send_usb_report( Adafruit_USBD_HID* hid, uint8_t * keyboard_report, uint8_t count);

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);

#endif // include guard
