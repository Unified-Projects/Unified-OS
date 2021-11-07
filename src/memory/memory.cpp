#include <memory/memory.h>

using namespace UnifiedOS;

//Fill a memory space with data
void Memory::memset(void* dest, uint8_t val, size_t size){
    //Loop over size
    for(size_t i = 0; i < size; i++){
        //Set address to value
        *(uint8_t*)((uint64_t)dest + i) = val;
    }
}

//Copy data from one memory address to another
void* Memory::memcpy(void* dest, void* src, size_t size){
    //Convert to uint8_t buffer pointers
    uint8_t *d = (uint8_t*)dest;

    //Lock any modification to the source
    const uint8_t *s = (uint8_t*)src;

    //Copy Data
    while (size-- > 0) {
        *d++ = *s++;
    }

    return (void*)d;
}