#ifndef __UNIFIED_OS_GDT_GDT_H
#define __UNIFIED_OS_GDT_GDT_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace GlobalDescriptorTable{
        //Descriptor Table
        struct GDTDescriptor {
            uint16_t Size;
            uint64_t Offset;
        } __attribute__((packed));

        //Descriptor Table entry
        struct GDTEntry {
            uint16_t Limit0;
            uint16_t Base0;
            uint8_t Base1;
            uint8_t AccessByte;
            uint8_t Limit1_Flags;
            uint8_t Base2;
        }__attribute__((packed));

        //TSS
        struct GDT64EntryTSS
        {
            uint16_t Length;
            uint16_t Low;
            uint8_t Middle;
            uint8_t AccessByte;
            uint8_t Limit1_Flags;
            uint8_t High;
            uint32_t High32;
            uint32_t Reserved;
        };
        

        //Global Descriptor Table
        struct GDT {
            GDTEntry Null; //0x00
            GDTEntry KernelCode; //0x08
            GDTEntry KernelData; //0x10
            GDTEntry UserCode; //0x18
            GDTEntry UserData; //0x20
            GDT64EntryTSS TSS; //0x28
        } __attribute__((packed)) 
        __attribute((aligned(0x1000)));

        extern GDT __GDT;
        extern GDTDescriptor __GDTDesc;
    }
}

//Links to the assembly loader
extern "C" void LoadGDT(UnifiedOS::GlobalDescriptorTable::GDTDescriptor* gdtDescriptor);

#endif