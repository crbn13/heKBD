#include "neopixel.hpp"
#include <sys/_stdint.h>

Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
RgbPixels rgb;

void neopixel_setup()
{
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();

  for (int i = 0 ; i < KEY_COUNT; i++)
  {
    rgb[i][0] = Pixel(30,0,0);
    rgb[i][1] = Pixel(10,10,10);
  }
}

void neopixel_update()
{
  pixels.clear();

  static uint8_t r = 0;
  static uint8_t g = 20;
  static uint8_t b = 40;
  static int rm    = 1;
  static int gm    = 1;
  static int bm    = 1;
  const int max    = 100;

  int startr = r;
  int startg = g;
  int startb = b;

  for (int i = 0; i < NEOPIXEL_COUNT; i++)
  {
    // if (i % 15 == 0 )
    {
      r += rm;
      g += gm;
      b += bm;
    }

    if (r > max - 1 || r < 0)
    {
      rm = rm * -1;
      r += rm;
    }
    if (g % max == 0)
    {
      gm *= -1;
      g += gm;
    }
    if (b % max == 0)
    {
      bm *= -1;
      b += bm;
    }

    pixels.setPixelColor(i, pixels.Color(r % max, 0, 0));
  }
  r = startr + rm;
  g = startg + gm;
  b = startb + bm;

  pixels.show(); // Send the updated pixel colors to the hardware.
}

Pixel::Pixel()
    : r(0)
    , g(0)
    , b(0)
{
}

Pixel::Pixel(const int _r, const int _g, const int _b)
    : r(_r)
    , g(_g)
    , b(_b)
{
}

Pixel& Pixel::operator=(const Pixel& other)
{
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
}

Pixel* RgbPixels::operator[](const std::size_t idx) 
{
    return pixels[idx];
}

void neopixel_update(const int fnLayer)
{
  for (int i = 0; i < NEOPIXEL_COUNT; i++)
  {
    pixels.setPixelColor(i, rgb[i][fnLayer].r, rgb[i][fnLayer].g, rgb[i][fnLayer].b);
  }
  pixels.show(); // Send the updated pixel colors to the hardware.
}