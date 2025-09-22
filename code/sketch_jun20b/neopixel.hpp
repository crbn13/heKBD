#include "config.hpp"
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

extern Adafruit_NeoPixel pixels;

struct Pixel
{
  uint8_t r, g, b;
  Pixel()
      : r(0)
      , g(0)
      , b(0)
};
struct RgbPixels
{
    Pixel pixels[NEOPIXEL_COUNT]; // array to store each pixels colour values

    // Apparently good practice to define both const and non const overloads
    const Pixel& operator[](const std::size_t idx);
    Pixel& operator[](std::size_t idx);

}

void
neopixel_setup();
void neopixel_update();
