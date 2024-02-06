#ifndef SDCARDHELPERS_H
#define SDCARDHELPERS_H

#include <SD.h>
#include <SPI.h>

class SDCardHelpers
{
private:
    const int chipSelect = BUILTIN_SDCARD;

public:
    bool initSD();
    void printDirectory(File dir, int numTabs);
    void printAllFiles();
    static bool writeTextFile(String filename, String message);
};

#endif