#include <sys/_stdint.h>
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

    static uint8_t r = 0;
    static uint8_t g = 20;
    static uint8_t b = 40;
    static int rm = 1;
    static int gm = 1;
    static int bm = 1;
    const int max = 100;

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

        if (r > max-1 || r < 0)
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

        pixels.setPixelColor(i, pixels.Color(r % max, 0,0));

    }
    r = startr + rm;
    g = startg + gm;
    b = startb + bm;
    

    pixels.show();   // Send the updated pixel colors to the hardware.

}
