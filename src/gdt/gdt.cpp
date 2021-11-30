#include <gdt/gdt.h>

using namespace UnifiedOS;
using namespace UnifiedOS::GlobalDescriptorTable;

//Default GDT to be loaded
__attribute__((aligned(0x1000)))
UnifiedOS::GlobalDescriptorTable::GDT UnifiedOS::GlobalDescriptorTable::__GDT = {
    {0, 0, 0, 0x00, 0x00, 0},         // null
    {0, 0, 0, 0x9a, 0xa0, 0},         // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0},         // kernel data segment
    {0, 0, 0, 0xFA, 0xCF, 0},         // User code segment
    {0, 0, 0, 0xF2, 0xCF, 0},         // User data segment
    {108, 0, 0, 0x89, 0xCF, 0, 0, 0}, // TSS Segment (add ADDRS)
};

//Global Descriptor to the gdt
GDTDescriptor UnifiedOS::GlobalDescriptorTable::__GDTDesc = {
    sizeof(__GDT), (uint64_t)(&__GDT)
};