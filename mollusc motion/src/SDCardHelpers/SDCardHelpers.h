#ifndef SDCARDHELPERS_H
#define SDCARDHELPERS_H

#include <SD.h>
#include <SPI.h>

class SDCardHelpers
{
private:
    static const int chipSelect;
    static uint32_t extmemBufferSize;
    static EXTMEM byte *extmemData;
    static uint8_t channelCount;
    static uint8_t keyframeDataSize; // in Bytes
    static uint8_t frameSizeInBytes;
    static String fileInMemory;

public:
    static bool initSD();
    static void initChannels(uint8_t count, uint8_t size);
    static void printDirectory(File dir, int numTabs);
    static void printAllFiles();
    static bool writeTextFile(String filename, String message);
    static bool loadFileToExtmem(String filename, uint32_t &frameCount);
    
    static void getFrame(uint16_t frame, uint16_t *buffer);
};

#endif