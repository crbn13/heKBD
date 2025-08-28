#ifndef GUARD_MULTIPLEXER_HPP
#define GUARD_MULTIPLEXER_HPP

#include "config.hpp"

#include <cstdint>
// Sets the 4 channel digital output to controll active multiplexer
void set_pins(const uint8_t value);

// Sets the active multiplexer if the values changed by more than 16
// Also waits 50 ms 
void set_multiplexer(const uint8_t value);

// sets up digital input and output pins
void setup_pins(); 

#endif // Include Guard
