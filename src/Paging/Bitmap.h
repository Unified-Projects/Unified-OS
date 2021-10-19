#pragma once

#include <stddef.h>
#include <stdint.h>

//Bitmap for Page Frame Allocators Free/Locked Pages
class Bitmap{
private:
public:
    size_t Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index);
    bool Get(uint64_t index);
    bool Set(uint64_t index, bool value);
};