#ifndef GUARD_KEY_HPP
#define GUARD_KEY_HPP

#include "config.hpp"


namespace KeyTypes
{
enum
{
  standard_actuation,
  rapid_trigger,
  analog_joystick,
  modifier_key,
  unassigned,
};

};
struct KeyValue // stores the keycodes and more of each switch, each element of this struct that will be an array will represent the same value as the keys[] array
{
  uint8_t keycode[FUNCTION_LAYERS];        // the HID_KEY value of the specific switch
  uint8_t key_type[FUNCTION_LAYERS];   // the type of key that the key is  
  uint8_t actuation_point; // the point at which the key achuates if its a standard_actuation key

// ¬¬¬¬¬¬¬¬¬¬¬ Analoge stuff : 
  uint8_t deadzone;
  // need "joystick" value / identifier
  int8_t* joystick_value; // pointer to gamepad struct member variable
  int8_t joystick_direction; // the sign modifier which sets the direction that the joystick goes in 
  
  
  // initializer 
  KeyValue() : keycode{0}, key_type{KeyTypes::rapid_trigger}, actuation_point(MAX_NORMALISED_ADC_VAL/2), deadzone(20), joystick_direction(1), joystick_value(std::nullptr_t()) { keycode[0] = HID_KEY_Z; }
};

struct Key {
  uint8_t normalised;     // number between 0-255 where 0 is unpressed and 255 is fully depressed
  uint16_t real;          // the value from ADC
  float min_real;         // minimum value read from ADC
  float max_real;         // value from ADC analog to digital conversion
  float factor;           // a number to map the un normalised values to 0-255 num
  bool active_state;      // The state that was last sent over usb
  int has_value_changed;  // If the value hasnt changed for a few frames and the key isnt pressed we can reset the min value

  Key()
      : normalised(0), real(0), min_real(MAX_ANALOG_VALUE),
        max_real(MIN_ANALOG_VALUE), factor(0.0F), active_state(0), has_value_changed(0)
  {
  }
};

extern Key keys[KEY_COUNT]; // initialize the array
extern KeyValue key_vals[KEY_COUNT]; 

#endif // Include Guard
