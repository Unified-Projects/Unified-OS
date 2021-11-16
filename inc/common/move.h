#ifndef __UNIFIED_OS_COMMON_MOVE_H
#define __UNIFIED_OS_COMMON_MOVE_H

#include <common/stdint.h>

namespace UnifiedOS{
    template<typename T>
    inline constexpr T&& move(T& t) {
        return static_cast<T&&>(t);
    }
}

#endif