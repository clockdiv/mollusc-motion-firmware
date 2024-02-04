#ifndef NEOPIXELS_H
#define NEOPIXELS_H

#include <Arduino.h>
#include <WS2812Serial.h>

#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define BLUE 0x000000FF
#define YELLOW 0x00FFD000
#define PINK 0x44F00080
#define ORANGE 0x00FF4200
#define WHITE 0xAA000000

class NeoPixels
{
private:
    static const int numled = 16;
    const int pin = 24;
    static byte drawingMemory[numled * 4];         //  4 bytes per LED for RGBW
    static DMAMEM byte displayMemory[numled * 16]; // 16 bytes per LED for RGBW
    WS2812Serial *leds;

public:
    NeoPixels();
    ~NeoPixels();
    void init();
    void set_all(int color, int b);
    void colorWipe(int color, int wait_us);
};

#endif