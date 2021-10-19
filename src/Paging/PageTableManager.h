#pragma once

#include "Paging.h"

class PageTableManager{
private:
public:
    PageTableManager(PageTable* PML4Address);
    PageTable* PML4;

    void MapMemory(void* virtualMemory, void* physicalMemory);
};

//Global Form
extern PageTableManager GlobalPTM;