#ifndef __UNIFIED_OS_PAGING_PAGE_FRAME_ALLOCATOR_H
#define __UNIFIED_OS_PAGING_PAGE_FRAME_ALLOCATOR_H

#include <boot/bootinfo.h>
#include <paging/bitmap.h>

namespace UnifiedOS{
    namespace Paging{
        //Allocator for memeory
        class PageFrameAllocator{
        private:
            void InitBitmap(size_t bitMapSize, void* bufferAddress);
            void ReservePage(void* address);
            void UnreservePage(void* address);
            void ReservePages(void* address, uint64_t pageCount);
            void UnreservePages(void* address, uint64_t pageCount);

        public:
            void ReadEFIMemoryMap(Boot::EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t DescriptorSize);
            Bitmap PageBitmap;
            void FreePage(void* address);
            void LockPage(void* address);
            void FreePages(void* address, uint64_t pageCount);
            void LockPages(void* address, uint64_t pageCount);

            void* RequestPage();
            void* RequestPages(uint64_t count);

            uint64_t GetFreeRAM();
            uint64_t GetUsedRAM();
            uint64_t GetReservedRAM();
        };

        //Global entry Page Frame Allocator
        extern PageFrameAllocator __PAGING__PFA_GLOBAL;

        //For memory sizes
        extern const uint64_t& __PAGING__TotalMemorySize__;
    }
}

#endif