#ifndef __UNIFIED_OS_PAGING_PAGE_MAP_INDEXER_H
#define __UNIFIED_OS_PAGING_PAGE_MAP_INDEXER_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Paging{
        class PageMapIndexer {
        private:
        public:
            PageMapIndexer(uint64_t virtualAddress);
            uint64_t PDP_i;
            uint64_t PD_i;
            uint64_t PT_i;
            uint64_t P_i;
        };
    }
}

#endif