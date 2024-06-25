#include "Settings.h"

Settings Setting;

const int Settings::chipSelect = BUILTIN_SDCARD;
const char* Settings::filename = "settings.json";

JsonDocument Settings::jsonDoc;

int Settings::setting1;
const char* Settings::setting2;
bool Settings::setting3;


Settings::Settings() {}

void Settings::initSD() {
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized for settings.");
  read();
}

void Settings::write() {

  jsonDoc["setting1"] = setting1;
  jsonDoc["setting2"] = setting2;
  jsonDoc["setting3"] = setting3;

  if (SD.exists(filename)) {
    SD.remove(filename);
  }

  File file = SD.open(filename, FILE_WRITE);
  if (file) {
    serializeJson(jsonDoc, file);
    file.close();
    Serial.printf("Settings saved to %s\n", filename);
  } else {
    Serial.println("Failed to open file for writing.");
  }
}

void Settings::read() {
  File file = SD.open(filename, FILE_WRITE);
  file = SD.open(filename);
  if (file) {
    JsonDocument readDoc;
    DeserializationError error = deserializeJson(readDoc, file);
    if (error) {
      Serial.printf("Failed to read file %s, error: ", filename);
      Serial.println(error.c_str());
    } else {
      setting1 = readDoc["setting1"];
      setting2 = readDoc["setting2"];
      setting3 = readDoc["setting3"];

      Serial.print("Setting1: ");
      Serial.println(setting1);
      Serial.print("Setting2: ");
      Serial.println(setting2);
      Serial.print("Setting3: ");
      Serial.println(setting3);
    }
    file.close();
  } else {
    Serial.println("Failed to open file for reading.");
  }
}
