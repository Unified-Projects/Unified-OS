#ifndef __UNIFIED_OS_PAGING_BITMAP_H
#define __UNIFIED_OS_PAGING_BITMAP_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Paging{
        class Bitmap{
        private:
        public:
            size_t Size;
            uint8_t* Buffer;
            bool operator[](uint64_t index);
            bool Get(uint64_t index);
            bool Set(uint64_t index, bool value);
        };
    }
}

#endif