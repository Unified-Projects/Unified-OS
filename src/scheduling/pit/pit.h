#pragma once

#include <stdint.h>

namespace PIT{
    extern double TimeSinceBoot;
    const uint64_t BaseFrequency = 1193182;

    void SleepS(double seconds);
    void SleepM(uint64_t milliseconds);

    void SetDivistor(uint16_t divisor);
    uint64_t GetFreqency();
    void SetFrequency(uint64_t frequency);
    void Tick();
}