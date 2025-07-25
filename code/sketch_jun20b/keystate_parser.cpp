#include "keystate_parser.hpp"

void parse_keys_and_send_usb()
{
  int active_layer = 0;
  
  set_pins(0);
  set_multiplexer(0);

  // used to avoid send multiple consecutive zero report for keyboard
  static bool keyPressedPreviously        = false;
  static bool controller_input_previously = false;
  bool controller_input                   = false;

  uint8_t count         = 0; // the number of keys being pressed
  uint8_t keycodes[6]   = { 0 }; // array of 6 keys that are being pressed
  bool modifier_changed = false;

  // check active keys and assign to keycode
  for (int i = 0; i < KEY_COUNT; i++)
  {
    keys[i].real = analogRead(ADC1); // reads analogue signal last

    if (i + 1 > 4) // temporary while doing silly things with 9 key keyboard
    {
      set_multiplexer((i + 1) % 5 + 16);
      set_pins((i + 1) % 5 + 16); // + 16 to skip to the next multiplexer
    }
    else
    {
      set_multiplexer(i + 1); // Set the multiplexer val first because it should be disabled before changing to the wrong key
      set_pins(i + 1);
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

    auto change_modifier = [&]()
    {
      if (keys[i].max_real != keys[i].min_real && keys[i].max_real - keys[i].min_real > 50) // only set modifier if
        keys[i].factor = (float)(NORMALISED_ADC_VAL)(-1) / (keys[i].max_real - keys[i].min_real);
      modifier_changed = false;
    };

    if (modifier_changed)
    {
      change_modifier();
    }

    decltype(keys[i].normalised) previous = +keys[i].normalised;

    keys[i].normalised = +int(abs((+keys[i].real - keys[i].min_real) * keys[i].factor));

    if (+int(abs((+keys[i].real - keys[i].min_real) * keys[i].factor)) > MAX_NORMALISED_ADC_VAL) // no cheeky buffer overflows
      keys[i].normalised = MAX_NORMALISED_ADC_VAL;

    switch (key_vals[i].key_type[active_layer])
    {
    case KeyTypes::rapid_trigger:
    {

      const int bounds_checker = 10; // needs renaming, it accounts for the random
      // variance in the analogue input

      const int top_bound     = 5;
      const int bottom_bound  = 25; // The inaccuracy betwen the bottom of the strok and the top
      const int change_buffer = 5;

      //  MOTHER OPTIMSATION
      // divide by the bounds checker and ignore the remainder which has the ish
      // effect as the bounds checker but... if its near the bound value it
      // wouldnt work

      // 0 IS NOT DEPRESSED, 255 is FULLY DEPRESSED
      if (keys[i].active_state)
      {
        keys[i].has_value_changed = 0;
        if (keys[i].normalised < bounds_checker && keys[i].normalised < +previous) // check if unpressed and key at top
        {
          keys[i].active_state = false;
          change_modifier();
        }
        else if (keys[i].normalised < +previous - change_buffer && keys[i].normalised < MAX_NORMALISED_ADC_VAL - bottom_bound) // check if direction of keystroke changed in middle of stroke
        {
          keys[i].active_state = false;
          keycodes[count++]    = key_vals[i].keycode[active_layer];
        }
        else if (keys[i].normalised >= MAX_NORMALISED_ADC_VAL - bottom_bound) // if its at the 255 ranges
        {
          keycodes[count++] = key_vals[i].keycode[active_layer];
        }
        else if (keys[i].normalised > previous) // check if key travelling downwards
        {
          keycodes[count++] = key_vals[i].keycode[active_layer];
        }
        else // if neither then set value to previous and switch still pressed
        {
          keycodes[count++]  = key_vals[i].keycode[active_layer];
          keys[i].normalised = previous;
        }
      }
      else // if key not already pressed
      {
        if (keys[i].normalised > +previous + change_buffer && keys[i].normalised > top_bound) // check that it has moved enough to achuate a press downwards
        {
          keys[i].has_value_changed = 0;
          keys[i].active_state      = true;
          keycodes[count++]         = key_vals[i].keycode[active_layer];
        }
        else if (keys[i].normalised > previous) // if the keystroke is going downwards then set the value to previous so that the distance gets bigger if it keeps going down
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
      if (int8_t(keys[i].normalised >> 1) >= abs(*key_vals[i].joystick_value))
      {
        *key_vals[i].joystick_value = int8_t(keys[i].normalised >> 1) * key_vals[i].joystick_direction;
        controller_input            = true;
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
      keys[i].min_real          = keys[i].real;
      keys[i].has_value_changed = 0;
      change_modifier();
    }

    if (count > 5) // usb hid has a max report of 6 keys at a time :(
      break; // break out of loop
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

  // send keyboard data :
  send_usb_report(&usb_keyboard, keycodes, count);

  if (controller_input)
  {
    send_usb_report(&usb_controller, &gamepad);

    controller_input            = false;
    controller_input_previously = true;
  }
  else
  {
    if (controller_input_previously)
    {
      send_usb_report(&usb_controller, &gamepad);
      controller_input_previously = false;
    }
  }
}
