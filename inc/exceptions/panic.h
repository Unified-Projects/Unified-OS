#ifndef __UNIFIED_OS_EXCEPTIONS_PANIC_H
#define __UNIFIED_OS_EXCEPTIONS_PANIC_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Exceptions{
        void Panic(const char* message);
        void Exception(uint8_t interrupt);
    }
}

#endif