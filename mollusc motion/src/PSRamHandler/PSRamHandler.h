#ifndef PSRAMHANDLER_H
#define PSRAMHANDLER_H

#include <Arduino.h>

extern "C" uint8_t external_psram_size;

class PSRamHandler
{
private:
    // test functions taken from https://www.pjrc.com/store/psram.html
    static uint32_t *memory_begin, *memory_end;
    static bool fail_message(volatile uint32_t *location, uint32_t actual, uint32_t expected);
    static bool check_fixed_pattern(uint32_t pattern);
    static bool check_lfsr_pattern(uint32_t seed);

public:
    PSRamHandler();
    static bool runTest();
};

#endif