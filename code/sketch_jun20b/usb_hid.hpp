#include "class/hid/hid.h"
#ifndef GUARD_USB_HID_HPP
#define GUARD_USB_HID_HPP

#include <Adafruit_TinyUSB.h>

#include "config.hpp"

// Report ID
enum {
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONTROLLER,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
};

// HID report descriptor using TinyUSB's template
extern uint8_t const desc_hid_report[]; 

// HID STUFF :
extern Adafruit_USBD_HID usb_hid;

// Gamepad stuff : 
extern hid_gamepad_report_t gamepad;
extern hid_mouse_report_t mouse;

void setup_usb();

void send_usb_report( Adafruit_USBD_HID* hid, hid_gamepad_report_t * report);
void send_usb_report( Adafruit_USBD_HID* hid, hid_mouse_report_t * report);

void send_usb_report( Adafruit_USBD_HID* hid, uint8_t * keyboard_report, uint8_t count);

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);

#endif // include guard
