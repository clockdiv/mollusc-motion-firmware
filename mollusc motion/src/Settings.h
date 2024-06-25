#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>




class Settings {
  private:
    static const int chipSelect;
    static JsonDocument jsonDoc;
    static const char* filename;

  public:
    Settings();

    void initSD();
    static void write();
    static void read();

    static int setting1;
    static const char* setting2;
    static bool setting3;

};
extern Settings Setting;

#endif
