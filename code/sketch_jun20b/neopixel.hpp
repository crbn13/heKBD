#include <Adafruit_NeoPixel.h>
#include "config.hpp"

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif



extern Adafruit_NeoPixel pixels;

void neopixel_setup();
void neopixel_update();

