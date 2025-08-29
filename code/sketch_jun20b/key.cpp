#include "key.hpp"

Key::Key()
      : normalised(0), real(0), min_real(MAX_ANALOG_VALUE),
        max_real(MIN_ANALOG_VALUE), factor(0.0F), active_state(0), has_value_changed(0)
  {
  }

KeyValue::KeyValue() : keycode{0}, key_type{KeyTypes::rapid_trigger}, actuation_point(MAX_NORMALISED_ADC_VAL/2), deadzone(20), joystick_direction(1), joystick_value(std::nullptr_t())
{
  for (int i = 0 ; i < FUNCTION_LAYERS ; i++)
  {
    keycode[i] = HID_KEY_Z;
  }
}

Key keys[KEY_COUNT];
KeyValue key_vals[KEY_COUNT]; 
