#pragma once

#include "../../filesystem/file.h"
#include <stdint.h>

struct tga_header {
    uint8_t IDLength; //length of Identifier
    uint8_t colormap; //Color Map ID
    uint8_t encoding; //Encoding / Compression type
    uint16_t cmaporig;//Origin of color map
    uint16_t cmaplen; //Length of color map
    uint8_t cmapdepth;//Color map depth
    uint16_t x;       //y origin //Only Supports 0 RN
    uint16_t y;       //x origin //Only Supports 0 RN
    uint16_t width;   //image's width
    uint16_t height;  //image's height
    uint8_t bpp;      //32 //if 24 dont use alpha or 1
    uint8_t imagedescriptor;
} __attribute__((packed));

struct TGA_Image
{
    tga_header header;
    uint32_t* Buffer;
    
} __attribute__((packed));

class TGA{
private:

    FILE::File* File;

    TGA_Image ReadTGA();

public:

    TGA(FILE::File* file);
    ~TGA();

    TGA_Image GetImage();
};