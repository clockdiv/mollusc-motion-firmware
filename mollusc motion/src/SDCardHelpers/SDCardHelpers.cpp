#include "SDCardHelpers.h"

void SDCardHelpers::initSD()
{
    if (!SD.begin(chipSelect))
    {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
}

void SDCardHelpers::printDirectory(File dir, int numTabs)
{
    while (true)
    {

        File entry = dir.openNextFile();
        if (!entry)
        {
            // no more files
            // Serial.println("**nomorefiles**");
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

void SDCardHelpers::printAllFiles()
{
    File root = SD.open("/");
    printDirectory(root, 0);
}

bool SDCardHelpers::writeTextFile(String filename, String message)
{
    File dataFile = SD.open(filename.c_str(), FILE_WRITE);

    if (!dataFile)
        return false;

    dataFile.println(message);
    dataFile.close();
    return true;
}