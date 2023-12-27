#ifndef MM_NEOPIXEL_H
#define MM_NEOPIXEL_H

#include <Arduino.h>
#include "pins.h"

void init_neopixels()
{
    leds.begin();
    leds.setBrightness(255); // 0=off, 255=brightest
}

void set_all_neopixels(int color, int b = 255)
{
    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, color);
    }
    leds.setBrightness(b);
    leds.show();
}

void colorWipe(int color, int wait_us)
{
    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, color);
        leds.show();
        delayMicroseconds(wait_us);
    }
}

#endif