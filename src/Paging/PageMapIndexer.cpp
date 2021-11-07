#include <paging/PageMapIndexer.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Paging;

//Setup and indexer by mapping a virtualAddress
PageMapIndexer::PageMapIndexer(uint64_t virtualAddress){
    virtualAddress >>= 12;
    P_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    PT_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    PD_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    PDP_i = virtualAddress & 0x1ff;
}