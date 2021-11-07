#include <paging/PageTableManager.h>
#include <paging/PageMapIndexer.h>
#include <paging/PageFrameAllocator.h>
#include <memory/memory.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Paging;
using namespace UnifiedOS::Memory;

PageTableManager UnifiedOS::Paging::__PAGING__PTM_GLOBAL = NULL;

//Load the Page table
PageTableManager::PageTableManager(PageTable* PML4Address){
    this->PML4 = PML4Address;
}

//Map a virtual memory address to a physical address
void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory){    
    // //Get the PDE
    // PDE = PML4->entries[indexer.PDP_i];
    
    // //Setup the PageTable
    // PageTable* PD;
    // if (!PDE.GetFlag(PT_Flag::Present)){ //If Entry is not present create it with read/write
    //     PD = (PageTable*)__PAGING__PFA_GLOBAL.RequestPage();
    //     memset(PD, 0, 0x1000);
    //     PDE.SetAddress((uint64_t)PD >> 12);
    //     PDE.SetFlag(PT_Flag::Present, true);
    //     PDE.SetFlag(PT_Flag::ReadWrite, true);
    //     PDP->entries[indexer.PD_i] = PDE;
    // }
    // else{ //Return Address Mapped
    //     PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    // }

    // //Get the PDE
    // PDE = PD->entries[indexer.PT_i];

    // //Setup the PageTable
    // PageTable* PT;
    // if (!PDE.GetFlag(PT_Flag::Present)){ //If Entry is not present create it with read/write
    //     PT = (PageTable*)__PAGING__PFA_GLOBAL.RequestPage();
    //     memset(PT, 0, 0x1000);
    //     PDE.SetAddress((uint64_t)PT >> 12);
    //     PDE.SetFlag(PT_Flag::Present, true);
    //     PDE.SetFlag(PT_Flag::ReadWrite, true);
    //     PD->entries[indexer.PT_i] = PDE;
    // }
    // else{ //Return Address Mapped
    //     PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    // }

    // //Create new entry and configure read/write
    // PDE = PT->entries[indexer.P_i];
    // PDE.SetAddress((uint64_t)physicalMemory >> 12);
    // PDE.SetFlag(PT_Flag::Present, true);
    // PDE.SetFlag(PT_Flag::ReadWrite, true);

    // //Load it
    // PT->entries[indexer.P_i] = PDE;

    //Get the indexer from virtuall address
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);    

    //Default the entry
    PageDirectoryEntry PDE;

    //Get the PDE
    PDE = PML4->entries[indexer.PDP_i];

    //Setup the page table
    PageTable* PDP;
    if (!PDE.GetFlag(PT_Flag::Present)){ //If Entry is not present create it with read/write
        PDP = (PageTable*)__PAGING__PFA_GLOBAL.RequestPage();
        memset(PDP, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4->entries[indexer.PDP_i] = PDE;
    }
    else{ //Return Address Mapped
        PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }
    
    //Get the PDE
    PDE = PDP->entries[indexer.PD_i];

    //Setup the page table
    PageTable* PD;
    if (!PDE.GetFlag(PT_Flag::Present)){ //If Entry is not present create it with read/write
        PD = (PageTable*)__PAGING__PFA_GLOBAL.RequestPage();
        memset(PD, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDP->entries[indexer.PD_i] = PDE;
    }
    else{ //Return Address Mapped
        PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }

    //Get the PDE
    PDE = PD->entries[indexer.PT_i];

    //Setup the page table
    PageTable* PT;
    if (!PDE.GetFlag(PT_Flag::Present)){ //If Entry is not present create it with read/write
        PT = (PageTable*)__PAGING__PFA_GLOBAL.RequestPage();
        memset(PT, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PD->entries[indexer.PT_i] = PDE;
    }
    else{ //Return Address Mapped
        PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
    }

    //Get the PDE
    PDE = PT->entries[indexer.P_i];
    
    //Set the pde address and flags and load to PT
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PT->entries[indexer.P_i] = PDE;
}