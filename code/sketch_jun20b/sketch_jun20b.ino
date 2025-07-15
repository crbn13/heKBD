// Stuff to change : 

#include "config.hpp"




#include <Adafruit_TinyUSB.h>

#include "key.hpp"
#include "multiplexer.hpp"




uint8_t const desc_hid_report_keyboard[] = {TUD_HID_REPORT_DESC_KEYBOARD()};
uint8_t const desc_hid_report_controller[] = {TUD_HID_REPORT_DESC_GAMEPAD()};

// HID STUFF :
Adafruit_USBD_HID usb_keyboard;
Adafruit_USBD_HID usb_controller;
//Adafruit_USBD_HID usb_mouse;

// Gamepad stuff : 
hid_gamepad_report_t gamepad;

// HZ controll stuff 
long start = 0;
long end = 100; // just in case div0 happens
float hz = 0;

Key keys[KEY_COUNT]; // initialize the array
KeyValue key_vals[KEY_COUNT]; 

void setup()
{
  Serial.begin(19200);

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
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // led pin
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  // set binary multiplexer output pins


  // set keycode values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // set keycode values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  key_vals[0].keycode[0] = HID_KEY_X;
  key_vals[1].keycode[0] = HID_KEY_C;
  key_vals[2].keycode[0] = HID_KEY_Y;
  
  //key_vals[3].keycode[0] = HID_KEY_D;
  key_vals[3].key_type[0] = KeyTypes::analog_joystick;
  key_vals[3].joystick_direction = -1;
  key_vals[3].joystick_value = &gamepad.x;
  //key_vals[3].

  key_vals[4].keycode[0] = HID_KEY_S;
  
  // key_vals[5].keycode[0] = HID_KEY_D;
  key_vals[5].key_type[0] = KeyTypes::analog_joystick;
  key_vals[5].joystick_direction = 1;
  key_vals[5].joystick_value = &gamepad.x;

  key_vals[6].keycode[0] = HID_KEY_Q;
  key_vals[7].keycode[0] = HID_KEY_W;
  key_vals[8].keycode[0] = HID_KEY_R;
  // need to actually make the custom keymap

  // for ( int i = 0 ; i < KEY_COUNT ; i++)
  // {
  // keys[i].keycode = keymap.key[]
  // }

  //analogReadResolution(12);

  // Set min vals :
  for (int i = 0; i < KEY_COUNT; i++) {
    keys[i].real = analogRead(ADC1);
    keys[i].min_real = keys[i].real;  // set the min value to the value at startup
  }
  // need to implement them being saved ofc

}  // end of setup


const int cycles = 1000;

void loop()
{
  start = micros(); // get time

  unsigned int inner_start = micros();

  for (int i = 0; i < cycles; i++)
  {
    inner_start = micros();

#ifdef TINYUSB_NEED_POLLING_TASK
    // Manual call tud_task since it isn't called by Core's background
    TinyUSBDevice.task();
#endif
    // not enumerated()/mounted() yet: nothing to do
    if (!TinyUSBDevice.mounted())
    {
      return;
    }
    // int val = analogRead(ADC1);
    // val = analogRead(ADC2);
    //while (!usb_keyboard.ready() || !usb_controller.ready()) { /* wait till its all done */ }

    process_hid();
    int timeDifference = 1000 - micros() + inner_start;
    if (timeDifference > 0)
      delayMicroseconds(timeDifference); 
  }
  /*
  Serial.print(keys[0].real);
  Serial.print("  ");
  Serial.print(keys[0].normalised);
  Serial.print("  ");
  Serial.print(keys[0].factor);
  Serial.print("  ");
  Serial.print(keys[0].max_real);
  Serial.print("  ");
  Serial.print(keys[0].min_real);
  Serial.print("  ");
  Serial.print(keys[0].active_state);
  Serial.print("  ");
  Serial.print(keys[0].has_value_changed);
  Serial.print("\t  |  ");

  Serial.print(keys[1].real);
  Serial.print("  ");
  Serial.print(keys[1].normalised);
  Serial.print("  ");
  Serial.print(keys[1].factor);
  Serial.print("  ");
  Serial.print(keys[1].max_real);
  Serial.print("  ");
  Serial.print(keys[1].min_real);
  Serial.print("  ");
  Serial.print(keys[1].active_state);
  Serial.print("  ");
  Serial.print(keys[1].has_value_changed);

  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    Serial.print(" | Pin ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(keys[i].real);
    Serial.print(" : ");
    Serial.print(keys[i].normalised);
  }
  Serial.print("\t| HZ = ");
  Serial.println(hz);
*/


  end = micros();
  // micros() is in micro seconds or E-6 of 1 second

  hz = 1.0f / (float(end - start) / (1000000.0F * float(cycles)));
  // ~~ Speeds notes : ~~ standard clock speed
  // when just doing the 2 analog reads it runs at : 116,813.56 hz
  // When printing each analog value to serial : 9,000 hz

  // Serial.print("\t| HZ = ");
  // Serial.println(hz);
}

void process_hid()
{


  int active_layer = 0;
  set_pins(0);
  set_multiplexer(0);

  // used to avoid send multiple consecutive zero report for keyboard
  static bool keyPressedPreviously = false;
  static bool controller_input_previously = false;
  bool controller_input = false;

  uint8_t count = 0;         // the number of keys being pressed
  uint8_t keycodes[6] = {0}; // array of 6 keys that are being pressed
  bool modifier_changed = false;

  // check active keys and assign to keycode
  for (int i = 0; i < KEY_COUNT; i++)
  {
    keys[i].real = analogRead(ADC1); // reads analogue signal last

    if ( i+1 > 4 ) // temporary while doing silly things with 9 key keyboard
    {
      set_multiplexer((i+1) % 5 + 16);
      set_pins((i+1) % 5 + 16); // + 16 to skip to the next multiplexer 
    }
    else
    {
      set_multiplexer(i+1); // Set the multiplexer val first because it should be disabled before changing to the wrong key
      set_pins(i+1);
    }

    modifier_changed = false;

    if (keys[i].max_real < keys[i].real)
    {
      keys[i].max_real = keys[i].real;
      modifier_changed = true;
    }
    if (keys[i].min_real > keys[i].real)
    {
      keys[i].min_real = keys[i].real;
      modifier_changed = true;
    }

    auto change_modifier = [&]() {
      if (keys[i].max_real != keys[i].min_real && keys[i].max_real - keys[i].min_real > 50)  // only set modifier if
        keys[i].factor = (float)(NORMALISED_ADC_VAL)(-1) / (keys[i].max_real - keys[i].min_real);
      modifier_changed = false;
    };

    if (modifier_changed)
    {
      change_modifier();
    }

    decltype(keys[i].normalised) previous = +keys[i].normalised;

    keys[i].normalised =
        +int(abs((+keys[i].real - keys[i].min_real) * keys[i].factor));

    if (+int(abs((+keys[i].real - keys[i].min_real) * keys[i].factor)) >
        MAX_NORMALISED_ADC_VAL) // no cheeky buffer overflows
      keys[i].normalised = MAX_NORMALISED_ADC_VAL;


    switch (key_vals[i].key_type[active_layer]) {
      case KeyTypes::rapid_trigger:
      {
const int bounds_checker = 10; // needs renaming, it accounts for the random
                                   // variance in the analogue input
                                  
    const int top_bound = 5;
    const int bottom_bound = 35; // The inaccuracy betwen the bottom of the strok and the top
    const int change_buffer = 5;

    //  MOTHER OPTIMSATION
    // divide by the bounds checker and ignore the remainder which has the ish
    // effect as the bounds checker but... if its near the bound value it
    // wouldnt work

    // 0 IS NOT DEPRESSED, 255 is FULLY DEPRESSED
    if (keys[i].active_state)
    {
      keys[i].has_value_changed = 0;
      if (keys[i].normalised < bounds_checker &&
          keys[i].normalised < +previous) // check if unpressed and key at top
      {
        keys[i].active_state = false;
        change_modifier();
      }
      else if (
          keys[i].normalised < +previous - change_buffer &&
          keys[i].normalised <
              MAX_NORMALISED_ADC_VAL -
                  bottom_bound) // check if direction of keystroke changed in
                                  // middle of stroke
      {
        keys[i].active_state = false;
        keycodes[count++] = key_vals[i].keycode[active_layer];
      }
      else if (
          keys[i].normalised >=
          MAX_NORMALISED_ADC_VAL - bottom_bound) // if its at the 255 ranges
      {
        keycodes[count++] = key_vals[i].keycode[active_layer];

      } else if (keys[i].normalised > previous)  // check if key travelling downwards
      {
        keycodes[count++] = key_vals[i].keycode[active_layer];
      }
      else // if neither then set value to previous and switch still pressed
      {
        keycodes[count++] = key_vals[i].keycode[active_layer];
        keys[i].normalised = previous;
      }
    }
    else // if key not already pressed
    {
      if (keys[i].normalised > +previous + change_buffer &&
          keys[i].normalised > top_bound) // check that it has moved enough
                                               // to achuate a press downwards
      {
        keys[i].has_value_changed = 0;
        keys[i].active_state = true;
        keycodes[count++] = key_vals[i].keycode[active_layer];
      } else if (keys[i].normalised > previous)  // if the keystroke is going downwards then set the value to previous so that the distance gets bigger if it keeps going down
      {
        keys[i].normalised = previous;
      }
      else // when keys.normalised is < previous the key is travelling upwards
      {
        if (keys[i].normalised == previous)
        {
          keys[i].has_value_changed++;
        }
        else
        {
          keys[i].has_value_changed = 0; // this means that it just doesnt work?
        }
      }
    }
    break;
      }
      case KeyTypes::standard_actuation:
      {
        if (keys[i].normalised > 100)
        {
          keycodes[count++] = key_vals[i].keycode[active_layer];
        }
        break;
      }
      case KeyTypes::analog_joystick:
      {
        if ( int8_t(keys[i].normalised >> 1) >= abs(*key_vals[i].joystick_value) )
        {
          *key_vals[i].joystick_value = int8_t(keys[i].normalised >> 1) * key_vals[i].joystick_direction;
          controller_input = true;
        }
      break;
      }
      default:
      {
        //nuffin to do here
      }
    
    }

    if (keys[i].has_value_changed > 200 && !keys[i].active_state)
    {
      keys[i].min_real = keys[i].real;
      keys[i].has_value_changed = 0;
      change_modifier();
    }

    if (count > 5) // usb hid has a max report of 6 keys at a time :(
      break;       // break out of loop
  } // For loop

  set_pins(0); // Sets the values for the next loop 
  set_multiplexer(0);

  if (TinyUSBDevice.suspended() && count)
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  // skip if hid is not ready e.g still transferring previous report

  

  if (controller_input)
  {
    usb_controller.sendReport(0, &gamepad, sizeof(gamepad));
    controller_input = false;
    controller_input_previously = true;
  }
  else
  {
    if (controller_input_previously)
      {
        usb_controller.sendReport(0, &gamepad, sizeof(gamepad));
        controller_input_previously = false; 
      }
  }
  gamepad.x = 0;
  gamepad.y = 0;
  gamepad.z = 0;
  gamepad.rz = 0;
  gamepad.rx = 0;
  gamepad.ry = 0;
  gamepad.hat = 0;
  gamepad.buttons = 0;

  if (count)
  {
    // Send report if there is key pressed
    uint8_t const report_id = 0;
    uint8_t const modifier = 0;  // modifier keys stored in array of 1 bit numbers

    keyPressedPreviously = true;
    usb_keyboard.keyboardReport(report_id, modifier, keycodes);
  }
  else
  {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if (keyPressedPreviously)
    {
      keyPressedPreviously = false;
      usb_keyboard.keyboardRelease(0);
    }

  }
}

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize)
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
