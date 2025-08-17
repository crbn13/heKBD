#include "neopixel.hpp"

Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);


void neopixel_setup()
{
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.clear();

}

void neopixel_update()
{
    pixels.clear();

    for (int i = 0; i < NEOPIXEL_COUNT; i++)
    {
        pixels.setPixelColor(i, pixels.Color(random() % 15, random() % 15, random() % 15));
    }

    pixels.show();   // Send the updated pixel colors to the hardware.

}
