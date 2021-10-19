#include "memory.h"

//From bootloader
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t DescriptorSize){
    static uint64_t memorySizeBytes = 0;
    if(memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * DescriptorSize));
        memorySizeBytes += desc->numPages * 4096;
    }

    return memorySizeBytes;
}

//Clear a buffer
void memset(void* start, uint8_t value, uint64_t num){
    for(uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

//Copy one buffers data to another
void* memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dst;
    const uint8_t *s = (uint8_t*)src;

    while (n-- > 0) {
        *d++ = *s++;
    }

    return d;
}