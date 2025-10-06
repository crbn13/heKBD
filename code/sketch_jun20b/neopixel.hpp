#include "config.hpp"
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

struct Pixel
{
  uint8_t r, g, b;

  Pixel();
  Pixel(const int _r, const int _g, const int _b);
  Pixel(const Pixel&);

  Pixel& operator=(const Pixel& other);
};

struct RgbPixels
{
  Pixel pixels[NEOPIXEL_COUNT][FUNCTION_LAYERS]; // array to store each pixels colour values

  // Apparently good practice to define both const and non const overloads
  Pixel* operator[](const std::size_t idx);

};

extern Adafruit_NeoPixel pixels;

extern RgbPixels rgb;

void neopixel_setup();
void neopixel_update();

void neopixel_update(const int fnLayer);
