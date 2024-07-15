#include "SDCardHelpers.h"

const int SDCardHelpers::chipSelect = BUILTIN_SDCARD;
uint32_t SDCardHelpers::extmemBufferSize = 0;
EXTMEM byte *SDCardHelpers::extmemData = NULL;
uint8_t SDCardHelpers::channelCount;
uint8_t SDCardHelpers::keyframeDataSize;
uint8_t SDCardHelpers::frameSizeInBytes;
String SDCardHelpers::fileInMemory = "";

bool SDCardHelpers::initSD()
{
    if (!SD.begin(chipSelect))
    {
        return false;
    }
    return true;
}

void SDCardHelpers::initChannels(uint8_t count, uint8_t size)
{
    channelCount = count;
    keyframeDataSize = size;
    frameSizeInBytes = channelCount * keyframeDataSize;
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

/// @brief Reads a binary file and puts it's content into EXTMEM.
/// The memory is allocated dynamically and has the same size as the file.
/// @param filename the binary file to read
bool SDCardHelpers::loadFileToExtmem(String filename, uint32_t &frameCount)
{
    Serial.printf("file currently in memory: %s\n", fileInMemory.c_str());
    Serial.printf("file to be loaded:        %s\n", filename.c_str());

    if (filename == fileInMemory)
    {
        Serial.println(F("File already read to memory."));
        return true;
    }
    else
    {
        if (fileInMemory != "")
        {
            Serial.println(F("Freeing memory"));
            extmem_free(extmemData);
        }
    }

    uint32_t extmemDataIndex = 0;

    File file = SD.open(filename.c_str());
    extmemBufferSize = file.size();

    frameCount = extmemBufferSize / frameSizeInBytes;

    // read the data from file and store it to the extmem buffer
    if (file)
    {
        extmemData = (byte *)extmem_malloc(extmemBufferSize);

        if (extmemData == NULL)
        {
            Serial.println(F("Unable to allocate memory."));
            return false;
        }
        while (file.available() >= frameSizeInBytes)
        {
            char result = file.read(extmemData + extmemDataIndex, frameSizeInBytes);
            extmemDataIndex += frameSizeInBytes;

            if (result < 0)
            {
                Serial.println(F("File smaller than expected"));
                return false;
            }
        }
        file.close();
    }
    else
    {
        Serial.println(F("File not found."));
        return false;
    }

    fileInMemory = filename;

    Serial.printf("Read file successfully\n");
    return true;
}

/// @brief reads a full frame from extmem to a buffer
/// @param frame the frame number to read
/// @param buffer the buffer to store the data to
void SDCardHelpers::getFrame(uint16_t frame, uint16_t *buffer)
{
    // Serial.println(F("get_frame_from_extmem"));
    uint32_t start = frame * frameSizeInBytes;
    for (uint8_t i = 0; i < frameSizeInBytes; i += 2)
    {
        char byteLow = extmemData[start + i];
        char byteHigh = extmemData[start + i + 1];
        uint16_t value = (byteHigh << 8) + byteLow;
        buffer[i / 2] = value;
    }
}
