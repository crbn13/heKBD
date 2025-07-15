#include "config.hpp"

#include <cstdint>
// Sets the 4 channel digital output to controll active multiplexer
void set_pins(const uint8_t value);

// Sets the active multiplexer
void set_multiplexer(const uint8_t value);

// sets up digital input and output pins
void setup_pins(); 
