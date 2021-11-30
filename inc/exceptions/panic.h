#ifndef __UNIFIED_OS_EXCEPTIONS_PANIC_H
#define __UNIFIED_OS_EXCEPTIONS_PANIC_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Exceptions{
        //Either panic (On purpose (Kernel))
        void Panic(const char* message);

        //Exception (kill a process)
        void Exception(uint8_t interrupt);
    }
}

#endif