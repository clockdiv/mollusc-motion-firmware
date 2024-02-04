#ifndef SDCARDHELPERS_H
#define SDCARDHELPERS_H

#include <SD.h>
#include <SPI.h>

class SDCardHelpers
{
private:
    const int chipSelect = BUILTIN_SDCARD;

public:
    void initSD();
    void printDirectory(File dir, int numTabs);
    void printAllFiles();
};

#endif