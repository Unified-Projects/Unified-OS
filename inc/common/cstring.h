#ifndef __UNIFIED_OS_COMMON_CSTRING_H
#define __UNIFIED_OS_COMMON_CSTRING_H

#include <common/stdint.h>

namespace UnifiedOS{
    //Conversions
    const char* to_string(uint64_t value);
    const char* to_string(int64_t value);

    //Hex Converter
    const char* to_hstring(uint64_t value);
    const char* to_hstring(uint32_t value);
    const char* to_hstring(uint16_t value);
    const char* to_hstring(uint8_t value);

    //Decimal Playce Converter
    const char* to_string(double value, uint8_t dp);
    const char* to_string(double value);

    //Copy a string
    // void strcpy(const char *dest, const char *src, size_t size);
    // void strcpy(const char *dest, const char *src);
}

//THESE SHOULD BE IN UNIFIED OS NAMESPACE IT JUST WAS NOT WORKING
    //Compare 2 Strings
    bool strcmp(const char* val1, const char* val2, size_t size);

    //Returns the length of a string
    int strlen(const char *str);

#endif