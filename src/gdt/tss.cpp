#include <gdt/tss.h>

#include <memory/memory.h>
#include <paging/PageFrameAllocator.h>
#include <paging/PageTableManager.h>

using namespace UnifiedOS;
using namespace UnifiedOS::GlobalDescriptorTable;
using namespace UnifiedOS::Memory;
using namespace UnifiedOS::Paging;

void GlobalDescriptorTable::InitTSS(TSS* tss){
    memset(tss, 0, sizeof(TSS));

    // Set up Interrupt Stack Tables
    tss->Ist1 = (uint64_t)__PAGING__PFA_GLOBAL.RequestPages(8);
    tss->Ist2 = (uint64_t)__PAGING__PFA_GLOBAL.RequestPages(8);
    tss->Ist3 = (uint64_t)__PAGING__PFA_GLOBAL.RequestPages(8);

    for (unsigned i = 0; i < 8; i++) {
        __PAGING__PTM_GLOBAL.MapMemory((void*)(tss->Ist1 + 8 * 0x1000), __PAGING__PFA_GLOBAL.RequestPage());
        __PAGING__PTM_GLOBAL.MapMemory((void*)(tss->Ist2 + 8 * 0x1000), __PAGING__PFA_GLOBAL.RequestPage());
        __PAGING__PTM_GLOBAL.MapMemory((void*)(tss->Ist3 + 8 * 0x1000), __PAGING__PFA_GLOBAL.RequestPage());
    }

    memset((void*)tss->Ist1, 0, 0x1000);
    memset((void*)tss->Ist2, 0, 0x1000);
    memset((void*)tss->Ist3, 0, 0x1000);

    tss->Ist1 += 0x1000 * 8;
    tss->Ist2 += 0x1000 * 8;
    tss->Ist3 += 0x1000 * 8;

    asm volatile("mov %%rsp, %0" : "=r"(tss->Rsp0));

    asm volatile("ltr %%ax" ::"a"(0x2B));
}

inline void GlobalDescriptorTable::SetKernelStack(TSS* tss, uint64_t stack){
    tss->Rsp0 = stack;
}