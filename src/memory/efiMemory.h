#pragma once

#include <stdint.h>

//Passed in from boot loader
struct EFI_MEMORY_DESCRIPTOR{
    uint32_t type;
    void* physAddr;
    void* virtAddr;
    uint64_t numPages;
    uint64_t attribs;
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];