#include "class/hid/hid.h"
#include "usb_hid.hpp"

// HID report descriptor using TinyUSB's template
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE   (HID_REPORT_ID(RID_MOUSE)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL))
};

Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gamepad;
hid_mouse_report_t mouse;

void setup_usb()
{
  if (!TinyUSBDevice.isInitialized())
  {
    TinyUSBDevice.begin(0);
  }

  // Setup KEYBOARD
  // usb_keyboard.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(1);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("tinyUSB composite kbd controller mouse");
  // Set up output report (on control endpoint) for Capslock indicator
  usb_hid.setReportCallback(NULL, hid_report_callback);
  usb_hid.begin();
  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted())
  {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}
// Sends Usb report for gamepad
void send_usb_report(Adafruit_USBD_HID* hid, hid_gamepad_report_t* report)
{
  hid->sendReport(RID_CONTROLLER, &gamepad, sizeof(gamepad));

  // reset gamepad input
  gamepad.x       = 0;
  gamepad.y       = 0;
  gamepad.z       = 0;
  gamepad.rz      = 0;
  gamepad.rx      = 0;
  gamepad.ry      = 0;
  gamepad.hat     = 0;
  gamepad.buttons = 0;
}

// Sends Usb report for keyboard
void send_usb_report(Adafruit_USBD_HID* hid, uint8_t* keyboard_report, uint8_t count)
{
  static bool keyPressedPreviously = false;

  if (count)
  {
    // Send report if there is key pressed
    uint8_t const report_id = RID_KEYBOARD;
    uint8_t const modifier  = 0; // modifier keys stored in array of 1 bit numbers

    keyPressedPreviously = true;
    hid->keyboardReport(report_id, modifier, keyboard_report);
  }
  else
  {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if (keyPressedPreviously)
    {
      keyPressedPreviously = false;
      hid->keyboardRelease(RID_KEYBOARD);
    }
  }
}
void send_usb_report( Adafruit_USBD_HID* hid, hid_mouse_report_t * report)
{
  hid->sendReport(RID_MOUSE, report, sizeof(*report));

  report->buttons = 0;
  report->pan = 0;
  report->wheel = 0;
  report->x = 0;
  report->y = 0;
}

void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void)report_id;
  (void)bufsize;

  // LED indicator is output report with only 1 byte length
  if (report_type != HID_REPORT_TYPE_OUTPUT)
    return;

  // The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
  // Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
  uint8_t ledIndicator = buffer[0];

#ifdef LED_BUILTIN
  // turn on LED if capslock is set
  digitalWrite(LED_BUILTIN, ledIndicator & KEYBOARD_LED_CAPSLOCK);
#endif
}
