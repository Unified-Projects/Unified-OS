#ifndef __UNIFIED_OS_INTERRUPTS_TIMER_PIT_H
#define __UNIFIED_OS_INTERRUPTS_TIMER_PIT_H

#include <common/stdint.h>
#include <interrupts/interrupts.h>

namespace UnifiedOS{
    namespace Interrupts{
        namespace Timer{
            class PIT : public InterruptHandler{
            protected:
                uint16_t Divisor = 65535; //Changeable to preference
                const uint64_t BaseFrequency = 1193182; //Wont change (Frequency of PIT chip)

                //PIT port
                IO::Port8BitSlow PITDataPort0;
                IO::Port8BitSlow PITCommandPort;
            public:
                //Constructors
                PIT(InterruptManager* manager);

                //Global Time
                double TimeSinceBoot;

                //Main Sleep
                void SleepS(double seconds);
                void SleepM(uint64_t milliseconds);

                //Configuring
                void SetDivistor(uint16_t divisor);
                uint64_t GetFreqency();
                void SetFrequency(uint64_t frequency);

                //Ticking
                virtual void HandleInterrupt(uint64_t rsp);
            };

            //Global Timer
            extern PIT* __TIMER__PIT__;
        }
    }
}

#endif