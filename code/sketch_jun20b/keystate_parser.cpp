#include "keystate_parser.hpp"
#include "key.hpp"

void parse_keys_and_send_usb()
{
  static uint8_t active_layer = 0;
  uint8_t next_active_layer = active_layer;
  static bool active_function_layers[FUNCTION_LAYERS] {0} ; // Used to record which function layer modifier keys are pressed.
  
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
    
    set_multiplexer(i + 1); // Set the multiplexer val first because it should be disabled before changing to the wrong key
    set_pins(i + 1);

    delayMicroseconds(10); // this stops interference between keys


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
      if (keys[i].max_real != keys[i].min_real && keys[i].max_real - keys[i].min_real > 200) // only set modifier if
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

    if (keys[i].active_state) // If the key is pressed down already 
    {
      
    }
    else // If The key is not already pressed down  
    {
      // key_vals[i].active_fn_layer = active_layer; // set the keys active layer 
    }

    switch (key_vals[i].key_type[key_vals[i].active_fn_layer])
    {

    case KeyTypes::rapid_trigger:
    {

      const int bounds_checker = 10; // needs renaming, it accounts for the random
      // variance in the analogue input

      const int top_bound     = 10;
      const int bottom_bound  = 35; // The inaccuracy betwen the bottom of the strok and the top
      const int change_buffer = 10;

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
          // keycodes[count++]    = key_vals[i].keycode[key_vals[i].active_fn_layer];
        }
        else if (keys[i].normalised >= MAX_NORMALISED_ADC_VAL - bottom_bound) // if its at the 255 ranges
        {
          keycodes[count++] = key_vals[i].keycode[key_vals[i].active_fn_layer];
        }
        else if (keys[i].normalised > previous) // check if key travelling downwards
        {
          keycodes[count++] = key_vals[i].keycode[key_vals[i].active_fn_layer];
        }
        else // if neither then set value to previous and switch still pressed
        {
          keycodes[count++]  = key_vals[i].keycode[key_vals[i].active_fn_layer];
          keys[i].normalised = previous;
        }
      }
      else // if key not already pressed
      {
        if (keys[i].normalised > +previous + change_buffer && keys[i].normalised > top_bound) // check that it has moved enough to achuate a press downwards
        {
          keys[i].has_value_changed = 0;
          keys[i].active_state      = true;
          key_vals[i].active_fn_layer = active_layer;
          keycodes[count++]         = key_vals[i].keycode[key_vals[i].active_fn_layer];
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
        key_vals[i].active_fn_layer = active_layer;
        keycodes[count++] = key_vals[i].keycode[key_vals[i].active_fn_layer];
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
    case KeyTypes::function_key:
    {
      if (keys[i].normalised > 255/2)
      {
        if (keys[i].active_state)
        {
        }
        else 
        {
          Serial.print("activated function layer is ");
          Serial.println(key_vals[i].keycode[key_vals[i].active_fn_layer]);
          key_vals[i].active_fn_layer = active_layer;
          active_function_layers[key_vals[i].keycode[key_vals[i].active_fn_layer]] = true;
          next_active_layer = key_vals[i].keycode[key_vals[i].active_fn_layer];
          keys[i].active_state = true;
        }
      }
      else 
      {
        if (keys[i].active_state)
        {
          active_function_layers[key_vals[i].keycode[key_vals[i].active_fn_layer]] = false;

          next_active_layer = 0;
          for (int x = 0 ; x < FUNCTION_LAYERS; x ++ )
          {
            if (active_function_layers[x])
              next_active_layer = x; 
          } // When modifier key released, sets to next highest modifier layer
            // might induce edge case when function layer goes to one below it
        }
        
        keys[i].active_state = false;
      }
      break;
    }
    case KeyTypes::unassigned:
    {
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
      break; // break out of 
  } // For loop

  active_layer = next_active_layer; // Set last to restrict keys checked aftter modifier key in series

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
