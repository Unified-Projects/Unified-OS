#pragma once

#include <stdint.h>

//Unused (For Now)
struct Color
{
    uint8_t Red = 0xff;
    uint8_t Green = 0xff;
    uint8_t Blue = 0xff;
    uint8_t Alpha = 0xff;

    uint32_t Get(){
        return (((uint32_t)Red   << 24) & 0xFF000000)|
               (((uint32_t)Green << 16) & 0x00FF0000)|
               (((uint32_t)Blue  << 8 ) & 0x0000FF00)|
               (((uint32_t)Alpha << 0 ) & 0x000000FF);
    }
};
