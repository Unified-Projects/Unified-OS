#ifndef __UNIFIED_OS_PAGING_PAGE_TABLE_MANAGER_H
#define __UNIFIED_OS_PAGING_PAGE_TABLE_MANAGER_H

#include <paging/paging.h>

namespace UnifiedOS{
    namespace Paging{
        class PageTableManager{
        private:
        public:
            PageTableManager(PageTable* PML4Address);
            PageTable* PML4;

            void MapMemory(void* virtualMemory, void* physicalMemory);
        };

        //Global Entry Page Table Manager
        extern PageTableManager __PAGING__PTM_GLOBAL;
    }
}

#endif