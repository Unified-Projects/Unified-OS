#include <gdt/tss.h>
#include <gdt/gdt.h>

#include <memory/memory.h>
#include <paging/PageFrameAllocator.h>
#include <paging/PageTableManager.h>

using namespace UnifiedOS;
using namespace UnifiedOS::GlobalDescriptorTable;
using namespace UnifiedOS::Memory;
using namespace UnifiedOS::Paging;

extern "C" void LoadTSS(uint64_t address, uint64_t gdt, uint8_t selector);

void GlobalDescriptorTable::InitTSS(TSS* tss, GDTDescriptor* gdt){
    // LoadTSS((uint64_t)tss, gdt->Offset, 0x28);

    GDT* gdtTable = (GDT*)(gdt->Offset);

    gdtTable->TSS.Length = 108;
    gdtTable->TSS.Low = ((uint64_t)tss) & 0xFFFF;
    gdtTable->TSS.Middle = ((uint64_t)tss >> 16) & 0xFF;
    gdtTable->TSS.High = ((uint64_t)tss >> 24) & 0xFF;
    gdtTable->TSS.High32 = ((uint64_t)tss >> 32) & 0xFFFFFFFF;
    gdtTable->TSS.Reserved = 0;

    memset(tss, 0, sizeof(TSS));

    // Set up Interrupt Stack Tables
    tss->Ist1 = (uint64_t)__PAGING__PFA_GLOBAL.RequestPages(8);
    tss->Ist2 = (uint64_t)__PAGING__PFA_GLOBAL.RequestPages(8);
    tss->Ist3 = (uint64_t)__PAGING__PFA_GLOBAL.RequestPages(8);

    memset((void*)tss->Ist1, 0, 0x1000);
    memset((void*)tss->Ist2, 0, 0x1000);
    memset((void*)tss->Ist3, 0, 0x1000);

    tss->Ist1 += 0x1000 * 8;
    tss->Ist2 += 0x1000 * 8;
    tss->Ist3 += 0x1000 * 8;

    asm volatile("mov %%rsp, %0" : "=r"(tss->Rsp0));

    asm volatile("ltr %%ax" ::"a"(0x28));
}

void GlobalDescriptorTable::SetKernelStack(TSS* tss, uint64_t stack){
    tss->Rsp0 = stack;
}