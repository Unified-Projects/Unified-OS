#ifndef __UNIFIED_OS_COMMON_STDIO_H
#define __UNIFIED_OS_COMMON_STDIO_H

#include <common/stdint.h>

namespace UnifiedOS{
    //Put pixels on the screen
    void putPix(uint32_t x, uint32_t y, uint32_t color);

    //Get a pixel data from the screen
    uint32_t getPix(uint32_t x, uint32_t y);

    //Fill screen with the color
    void Clear(uint32_t color);

    //Shift the screen up
    void ShiftUp();

    //Skip one line down
    void Next();

    //Put a character on the screen at any pos
    void putChar(unsigned int color, char chr, unsigned int xOff, unsigned int yOff);

    //Put a character on the screen at cusor pos
    void putChar(char chr, unsigned int color);

    //Print a string on the screen
    void printf(const char* str);

    //Clear the position (0, 0)
    void ClearPos();

    //Add to x and y
    void OffsetPos(int x, int y);

    //Set specific positions
    void SetPosX(uint32_t x);
    void SetPosY(uint32_t y);

    //Get specific postitions
    uint32_t GetXPos();
    uint32_t GetYPos();
}

#endif