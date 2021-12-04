#ifndef __UNIFIED_OS_MEMORY_MEMORY_H
#define __UNIFIED_OS_MEMORY_MEMORY_H

#include <common/stdint.h>

namespace UnifiedOS
{
    namespace Memory{
        //Fill a memory space with data
        void memset(void* dest, uint8_t val, size_t size);

        //Copy data from one memory address to another
        void* memcpy(void* dest, void* src, size_t size);

        //Memory Comparison
        bool memcmp(const void* Mem1, const void* Mem2, size_t Size);
    }
}


#endif