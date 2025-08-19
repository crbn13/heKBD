#include "usb_hid.hpp"

uint8_t const desc_hid_report_keyboard[]   = { TUD_HID_REPORT_DESC_KEYBOARD() };
uint8_t const desc_hid_report_controller[] = { TUD_HID_REPORT_DESC_GAMEPAD () };

Adafruit_USBD_HID usb_keyboard;
Adafruit_USBD_HID usb_controller;

hid_gamepad_report_t gamepad;

void setup_usb()
{
  if (!TinyUSBDevice.isInitialized())
  {
    TinyUSBDevice.begin(0);
  }

  // Setup KEYBOARD
  // usb_keyboard.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_keyboard.setPollInterval(1);
  usb_keyboard.setReportDescriptor(desc_hid_report_keyboard, sizeof(desc_hid_report_keyboard));
  usb_keyboard.setStringDescriptor("tinyUSB Keyboard");
  // Set up output report (on control endpoint) for Capslock indicator
  usb_keyboard.setReportCallback(NULL, hid_report_callback);
  usb_keyboard.begin();

  // Setup Controller

  usb_controller.setPollInterval(1);
  usb_controller.setReportDescriptor(desc_hid_report_controller, sizeof(desc_hid_report_controller));
  usb_controller.setStringDescriptor("tinyUSB Controller");
  usb_controller.begin();

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
  hid->sendReport(0, &gamepad, sizeof(gamepad));

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
    uint8_t const report_id = 0;
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
      hid->keyboardRelease(0);
    }
  }
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
