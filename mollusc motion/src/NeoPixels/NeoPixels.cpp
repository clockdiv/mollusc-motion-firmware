#include "NeoPixels.h"

const int NeoPixels::numled;
byte NeoPixels::drawingMemory[NeoPixels::numled * 4];
DMAMEM byte NeoPixels::displayMemory[NeoPixels::numled * 16];

NeoPixels::NeoPixels()
{
    leds = new WS2812Serial(numled, &displayMemory, &drawingMemory, pin, WS2812_GRBW);
}

NeoPixels::~NeoPixels()
{
    delete (leds);
}

void NeoPixels::init()
{
    leds->begin();
    leds->setBrightness(255); // 0=off, 255=brightest
}

void NeoPixels::set_all(int color, int b = 255)
{
    for (int i = 0; i < leds->numPixels(); i++)
    {
        leds->setPixel(i, color);
    }
    leds->setBrightness(b);
    leds->show();
}

void NeoPixels::colorWipe(int color, int wait_us)
{
    for (int i = 0; i < leds->numPixels(); i++)
    {
        leds->setPixel(i, color);
        leds->show();
        delayMicroseconds(wait_us);
    }
}
