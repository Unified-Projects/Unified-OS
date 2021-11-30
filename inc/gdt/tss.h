#ifndef __UNIFIED_OS_GDT_TSS_H
#define __UNIFIED_OS_GDT_TSS_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace GlobalDescriptorTable{
        struct GDTDescriptor;

        //Task Segment
        struct TSS {
            uint32_t Reserved __attribute__((aligned(16)));
            uint64_t Rsp0;
            uint64_t Rsp1;
            uint64_t Rsp2;
            uint64_t Reserved2;
            uint64_t Ist1;
            uint64_t Ist2;
            uint64_t Ist3;
            uint64_t Ist4;
            uint64_t Ist5;
            uint64_t Ist6;
            uint64_t Ist7;
            uint64_t Reserved3;
            uint32_t Reserved4;
            uint32_t IopbOffset;
        } __attribute__((packed));

        //TSS setup
        void InitTSS(TSS* tss, GDTDescriptor* gdt);

        //Kernel stack on the tss
        void SetKernelStack(TSS* tss, uint64_t stack);
    }
}

#endif