#ifndef __UNIFIED_OS_SMP_SMP_H
#define __UNIFIED_OS_SMP_SMP_H

#include <common/stdint.h>

#include <gdt/gdt.h>
#include <IO/CPU/CPU.h>

namespace UnifiedOS{
    namespace SMP{
        //Setup
        void Intitialise();

        //CPUs
        extern uint8_t ActiveCPUs;
        extern CPU::CPU* CPUs[256];
        //Main tss
        extern GlobalDescriptorTable::TSS tss1;
    }
}

#endif