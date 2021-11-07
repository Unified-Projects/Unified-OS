#ifndef __UNIFIED_OS_COMMON_STDIO_H
#define __UNIFIED_OS_COMMON_STDIO_H

#include <common/stdint.h>

namespace UnifiedOS{
    void putPix(uint32_t x, uint32_t y, uint32_t color);

    uint32_t getPix(uint32_t x, uint32_t y);

    void Clear(uint32_t color);

    void ShiftUp();

    void Next();

    void putChar(unsigned int color, char chr, unsigned int xOff, unsigned int yOff);

    void putChar(char chr, unsigned int color);

    void printf(const char* str);

    // void printfHex(uint8_t val){
    //     char* ret = "0x  ";
    //     char* hex = "0123456789ABCDEF";
    //     ret[2] = hex[(val >> 4) & 0xF];
    //     ret[3] = hex[val & 0xF];
    //     printf(ret);
    // }

}

#endif