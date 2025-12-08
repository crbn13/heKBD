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
  function_key,
  macro_key,
  unassigned,
};

};
struct KeyValue // stores the keycodes and more of each switch, each element of this struct that will be an array will represent the same value as the keys[] array
{
  uint8_t keycode[FUNCTION_LAYERS];        // the HID_KEY value of the specific switch. Also used for function layer
  uint8_t key_type[FUNCTION_LAYERS];   // the type of key that the key is  
  uint8_t actuation_point; // the point at which the key achuates if its a standard_actuation key
  uint8_t active_fn_layer; // The active function layer that was active when the key was initially depressed

// ¬¬¬¬¬¬¬¬¬¬¬ Analoge stuff : 
  uint8_t deadzone;
  // need "joystick" value / identifier
  int8_t* joystick_value; // pointer to gamepad struct member variable
  int8_t joystick_direction; // the sign modifier which sets the direction that the joystick goes in 
  
  
  // initializer 
  KeyValue();
};

constexpr int PAST_READING_COUNT = 5;

struct Key {
  uint8_t normalised;     // number between 0-255 where 0 is unpressed and 255 is fully depressed
  uint16_t real;          // the value from ADC
  float min_real;         // minimum value read from ADC
  float max_real;         // high band value from ADC analog to digital conversion
  float factor;           // a number to map the un normalised values to 0-255 num
  bool active_state;      // The state that was last sent over usb
  int has_value_changed;  // If the value hasnt changed for a few frames and the key isnt pressed we can reset the min value
  uint16_t past_readings[PAST_READING_COUNT]; // the last 5 values read

  Key();
};

extern Key keys[KEY_COUNT]; // initialize the array
extern KeyValue key_vals[KEY_COUNT]; 

#endif // Include Guard
