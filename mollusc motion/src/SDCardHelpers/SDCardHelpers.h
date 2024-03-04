#ifndef SDCARDHELPERS_H
#define SDCARDHELPERS_H

#include <SD.h>
#include <SPI.h>

#define FRAME_DATA_SIZE 28

class SDCardHelpers
{
private:
    static const int chipSelect;
    static uint32_t extmem_buffer_size;
    static EXTMEM byte *extmem_data;

public:
    static bool initSD();
    static void printDirectory(File dir, int numTabs);
    static void printAllFiles();
    static bool writeTextFile(String filename, String message);
    static bool loadFileToExtmem(String filename);
    static void getFrame(uint16_t frame, uint16_t *buffer);
};

#endif