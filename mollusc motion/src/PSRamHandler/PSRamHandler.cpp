#include "PSRamHandler.h"
uint32_t *PSRamHandler::memory_begin, *PSRamHandler::memory_end;

PSRamHandler::PSRamHandler()
{
}

bool PSRamHandler::runTest()
{
    uint8_t size = external_psram_size;
    Serial.printf("EXTMEM Memory Test, %d Mbyte\n", size);
    if (size == 0)
        return false;
    const float clocks[4] = {396.0f, 720.0f, 664.62f, 528.0f};
    const float frequency = clocks[(CCM_CBCMR >> 8) & 3] / (float)(((CCM_CBCMR >> 29) & 7) + 1);
    Serial.printf(" CCM_CBCMR=%08X (%.1f MHz)\n", CCM_CBCMR, frequency);
    memory_begin = (uint32_t *)(0x70000000);
    memory_end = (uint32_t *)(0x70000000 + size * 1048576);
    elapsedMillis msec = 0;
    if (!check_fixed_pattern(0x5A698421))
        return false;
    if (!check_lfsr_pattern(2976674124ul))
        return false;
    if (!check_lfsr_pattern(1438200953ul))
        return false;
    if (!check_lfsr_pattern(3413783263ul))
        return false;
    if (!check_lfsr_pattern(1900517911ul))
        return false;
    if (!check_lfsr_pattern(1227909400ul))
        return false;
    if (!check_lfsr_pattern(276562754ul))
        return false;
    if (!check_lfsr_pattern(146878114ul))
        return false;
    if (!check_lfsr_pattern(615545407ul))
        return false;
    if (!check_lfsr_pattern(110497896ul))
        return false;
    if (!check_lfsr_pattern(74539250ul))
        return false;
    if (!check_lfsr_pattern(4197336575ul))
        return false;
    if (!check_lfsr_pattern(2280382233ul))
        return false;
    if (!check_lfsr_pattern(542894183ul))
        return false;
    if (!check_lfsr_pattern(3978544245ul))
        return false;
    if (!check_lfsr_pattern(2315909796ul))
        return false;
    if (!check_lfsr_pattern(3736286001ul))
        return false;
    if (!check_lfsr_pattern(2876690683ul))
        return false;
    if (!check_lfsr_pattern(215559886ul))
        return false;
    if (!check_lfsr_pattern(539179291ul))
        return false;
    if (!check_lfsr_pattern(537678650ul))
        return false;
    if (!check_lfsr_pattern(4001405270ul))
        return false;
    if (!check_lfsr_pattern(2169216599ul))
        return false;
    if (!check_lfsr_pattern(4036891097ul))
        return false;
    if (!check_lfsr_pattern(1535452389ul))
        return false;
    if (!check_lfsr_pattern(2959727213ul))
        return false;
    if (!check_lfsr_pattern(4219363395ul))
        return false;
    if (!check_lfsr_pattern(1036929753ul))
        return false;
    if (!check_lfsr_pattern(2125248865ul))
        return false;
    if (!check_lfsr_pattern(3177905864ul))
        return false;
    if (!check_lfsr_pattern(2399307098ul))
        return false;
    if (!check_lfsr_pattern(3847634607ul))
        return false;
    if (!check_lfsr_pattern(27467969ul))
        return false;
    if (!check_lfsr_pattern(520563506ul))
        return false;
    if (!check_lfsr_pattern(381313790ul))
        return false;
    if (!check_lfsr_pattern(4174769276ul))
        return false;
    if (!check_lfsr_pattern(3932189449ul))
        return false;
    if (!check_lfsr_pattern(4079717394ul))
        return false;
    if (!check_lfsr_pattern(868357076ul))
        return false;
    if (!check_lfsr_pattern(2474062993ul))
        return false;
    if (!check_lfsr_pattern(1502682190ul))
        return false;
    if (!check_lfsr_pattern(2471230478ul))
        return false;
    if (!check_lfsr_pattern(85016565ul))
        return false;
    if (!check_lfsr_pattern(1427530695ul))
        return false;
    if (!check_lfsr_pattern(1100533073ul))
        return false;
    if (!check_fixed_pattern(0x55555555))
        return false;
    if (!check_fixed_pattern(0x33333333))
        return false;
    if (!check_fixed_pattern(0x0F0F0F0F))
        return false;
    if (!check_fixed_pattern(0x00FF00FF))
        return false;
    if (!check_fixed_pattern(0x0000FFFF))
        return false;
    if (!check_fixed_pattern(0xAAAAAAAA))
        return false;
    if (!check_fixed_pattern(0xCCCCCCCC))
        return false;
    if (!check_fixed_pattern(0xF0F0F0F0))
        return false;
    if (!check_fixed_pattern(0xFF00FF00))
        return false;
    if (!check_fixed_pattern(0xFFFF0000))
        return false;
    if (!check_fixed_pattern(0xFFFFFFFF))
        return false;
    if (!check_fixed_pattern(0x00000000))
        return false;
    Serial.printf("Test ran for %.2f seconds\n", (float)msec / 1000.0f);
    return true;
}

bool PSRamHandler::fail_message(volatile uint32_t *location, uint32_t actual, uint32_t expected)
{
    Serial.printf(" Error at %08X, read %08X but expected %08X\n", (uint32_t)location, actual, expected);
    return false;
}

bool PSRamHandler::check_fixed_pattern(uint32_t pattern)
{
    volatile uint32_t *p;
    Serial.printf("testing with fixed pattern %08X\n", pattern);
    for (p = memory_begin; p < memory_end; p++)
    {
        *p = pattern;
    }
    arm_dcache_flush_delete((void *)memory_begin,
                            (uint32_t)memory_end - (uint32_t)memory_begin);
    for (p = memory_begin; p < memory_end; p++)
    {
        uint32_t actual = *p;
        if (actual != pattern)
            return fail_message(p, actual, pattern);
    }
    return true;
}

bool PSRamHandler::check_lfsr_pattern(uint32_t seed)
{
    volatile uint32_t *p;
    uint32_t reg;

    Serial.printf("testing with pseudo-random sequence, seed=%u\n", seed);
    reg = seed;
    for (p = memory_begin; p < memory_end; p++)
    {
        *p = reg;
        for (int i = 0; i < 3; i++)
        {
            // https://en.wikipedia.org/wiki/Xorshift
            reg ^= reg << 13;
            reg ^= reg >> 17;
            reg ^= reg << 5;
        }
    }
    arm_dcache_flush_delete((void *)memory_begin,
                            (uint32_t)memory_end - (uint32_t)memory_begin);
    reg = seed;
    for (p = memory_begin; p < memory_end; p++)
    {
        uint32_t actual = *p;
        if (actual != reg)
            return fail_message(p, actual, reg);
        // Serial.printf(" reg=%08X\n", reg);
        for (int i = 0; i < 3; i++)
        {
            reg ^= reg << 13;
            reg ^= reg >> 17;
            reg ^= reg << 5;
        }
    }
    return true;
}
