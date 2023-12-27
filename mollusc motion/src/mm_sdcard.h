#ifndef MM_SDCARD_H
#define MM_SDCARD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "pins.h"

void printDirectory(File dir, int numTabs)
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

void log_sd(String dataString)
{
    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    dataString = String(millis()) + ", " + dataString;

    // if the file is available, write to it:
    if (dataFile)
    {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else
    {
        Serial.println("error opening datalog.txt");
    }
}

void init_sd()
{
    if (!SD.begin(chipSelect))
    {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    root = SD.open("/");

    printDirectory(root, 0);
}

#endif