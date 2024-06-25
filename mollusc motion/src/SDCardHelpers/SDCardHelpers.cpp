#include "SDCardHelpers.h"

const int SDCardHelpers::chipSelect = BUILTIN_SDCARD;
uint32_t SDCardHelpers::extmem_buffer_size = 0;
EXTMEM byte *SDCardHelpers::extmem_data;

bool SDCardHelpers::initSD()
{
    if (!SD.begin(chipSelect))
    {
        return false;
    }
    return true;
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
bool SDCardHelpers::loadFileToExtmem(String filename)
{
    Serial.print(F("read_file_to_extmem: "));
    Serial.println(filename);

    uint32_t extmem_data_index = 0;

    // char filename_c[12];
    // strcpy(filename_c, filename.c_str());
    File file = SD.open(filename.c_str());
    extmem_buffer_size = file.size();

    Serial.print(F("File Size: "));
    Serial.println(extmem_buffer_size);

    // read the data from file and store it to the extmem buffer
    if (file)
    {
        extmem_data = (byte *)extmem_malloc(extmem_buffer_size);

        if (extmem_data == NULL)
        {
            Serial.println("Unable to allocate memory.");
            return false;
        }
        while (file.available() >= FRAME_DATA_SIZE)
        {
            char result = file.read(extmem_data + extmem_data_index, FRAME_DATA_SIZE);
            extmem_data_index += FRAME_DATA_SIZE;

            if (result < 0)
            {
                Serial.println("file smaller than expected");
                return false;
            }
        }
        file.close();
    }
    else
    {
        Serial.println("File not found.");
        return false;
    }
    return true;
}

/// @brief reads a full frame from extmem to a buffer
/// @param frame the frame number to read
/// @param buffer the buffer to store the data to
void SDCardHelpers::getFrame(uint16_t frame, uint16_t *buffer)
{
    // Serial.println(F("get_frame_from_extmem"));
    uint32_t start = frame * FRAME_DATA_SIZE;
    for (uint8_t i = 0; i < FRAME_DATA_SIZE; i += 2)
    {
        char byteLow = extmem_data[start + i];
        char byteHigh = extmem_data[start + i + 1];
        uint16_t value = (byteHigh << 8) + byteLow;
        buffer[i / 2] = value;
    }
}
