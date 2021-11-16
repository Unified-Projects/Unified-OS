#ifndef __UNIFIED_OS_COMMON_TTRAITS_H
#define __UNIFIED_OS_COMMON_TTRAITS_H

#include <common/stdint.h>

template <typename T> class TTraits {
public:
    // A type is trivial if the constructor is defaulted (implicitly defined or explicity defaulted)
    // Trivial types can be copied with memcpy, etc.
    // This is important for Vector, etc.
    inline static constexpr bool is_trivial() { return false; }

    inline static constexpr const char* name() { return "undefined"; }
};

template <> class TTraits<char> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "char/int8_t"; }
};

// template <> class TTraits<int8_t> {
// public:
//     inline static constexpr bool is_trivial() { return true; }

//     inline static constexpr const char* name() { return "int8_t"; }
// };

template <> class TTraits<uint8_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "uint8_t"; }
};

template <> class TTraits<int16_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "int16_t"; }
};

template <> class TTraits<uint16_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "uint16_t"; }
};

template <> class TTraits<int32_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "int32_t"; }
};

template <> class TTraits<uint32_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "uint32_t"; }
};

template <> class TTraits<int64_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "int64_t"; }
};

template <> class TTraits<uint64_t> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "uint64_t"; }
};

template <typename T> class TTraits<T*> {
public:
    inline static constexpr bool is_trivial() { return true; }

    inline static constexpr const char* name() { return "T*"; }
};

#endif