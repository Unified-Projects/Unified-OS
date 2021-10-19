#pragma once

#include <stdint.h>

//QUERTY US
namespace QWERTYKeyboard{
    #define LeftShift 0x2A
    #define RightShift 0x36
    #define Enter 0x1C
    #define BackSpace 0x0E
    #define Spacebar 0x39

    extern const char ASCITable[];

    char Translate(uint8_t scanCode, bool uppercase);
}