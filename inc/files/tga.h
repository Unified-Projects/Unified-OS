#pragma once

#include <fs/ByteInteraction.h>
using namespace UnifiedOS::FileSystem::Byte;
using namespace UnifiedOS::FileSystem::Endian;

#include <common/stdint.h>

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

    TGA_Image ReadTGA(uint8_t* Buffer){

        tga_header header = {};
        TGA_Image image = {};

        uint32_t* ImageDataBuffer;

        //Type of TGA //We want 0
        header.IDLength = GetByte(Buffer, 0x00);
        offsetBuffer(Buffer, 1);

        header.colormap = GetByte(Buffer, 0x00);
        offsetBuffer(Buffer, 1);

        header.encoding = GetByte(Buffer, 0x00);
        offsetBuffer(Buffer, 1);

        //Start of data map
        header.cmaporig = LittleEndian(Get2Byte(Buffer, 0x00));
        offsetBuffer(Buffer, 2);

        //Length of image data map
        header.cmaplen = LittleEndian(Get2Byte(Buffer, 0x00));
        offsetBuffer(Buffer, 2);
        
        //Depth of color map
        header.cmapdepth = GetByte(Buffer, 0x00);
        offsetBuffer(Buffer, 1);

        header.x = LittleEndian(Get2Byte(Buffer, 0x00));
        offsetBuffer(Buffer, 2);

        header.y = LittleEndian(Get2Byte(Buffer, 0x00));
        offsetBuffer(Buffer, 2);

        header.width = LittleEndian(Get2Byte(Buffer, 0x00));
        offsetBuffer(Buffer, 2);
        
        header.height = LittleEndian(Get2Byte(Buffer, 0x00));
        offsetBuffer(Buffer, 2);

        header.bpp = GetByte(Buffer, 0x00);
        offsetBuffer(Buffer, 1);

        header.imagedescriptor = GetByte(Buffer, 0x00);
        offsetBuffer(Buffer, 1);

        offsetBuffer(Buffer, header.IDLength);

        //Assign Data storage to buffer
        ImageDataBuffer = new uint32_t[header.width * header.height];

        //More info http://www.paulbourke.net/dataformats/tga/
        switch (header.encoding)
        {
        case 2: //Uncompressed
            //Switch pixel types
            switch (header.bpp)
            {
            case 16: //(Bits) ARRR RRGG GGGB BBBB
                /* code */
                break;

            case 24: //RR GG BB
                for(int y = 0; y < header.height; y++){
                    for(int x = 0; x < header.width; x++){
                        // ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = (Get4Byte(Buffer, 0x00) >> 8) | (0xFF << 24);
                        // ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = 0xFF000000 | (Get4Byte(Buffer, 0x00) >> 8);
                        uint32_t Color = 0;
                        Color |= 0xFF << 24;
                        Color |= GetByte(Buffer, 0x00) << 0;
                        Color |= GetByte(Buffer, 0x01) << 8;
                        Color |= GetByte(Buffer, 0x02) << 16;
                        ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = Color;
                        offsetBuffer(Buffer, 3);
                    }
                }
                break;
            
            case 32: //AA RR GG BB
                for(int y = 0; y < header.height; y++){
                    for(int x = 0; x < header.width; x++){
                        ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = (Get4Byte(Buffer, 0x00) >> 8) | (GetByte(Buffer, 3) << 24);
                        // ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = Get4Byte(Buffer, 0x00);
                        offsetBuffer(Buffer, 4);
                    }
                }
                break;
            }
            break;
        
        case 9: //Run Length Encoded, Color mapped
            break;

        case 10: //Run Length Encoded, RGB images
            break;
        }

        image.Buffer = ImageDataBuffer;
        image.header = header;

        return image;
    }

public:

    TGA(){

    }
    ~TGA(){

    }

    TGA_Image GetImage(uint8_t* Buffer){
        TGA_Image image;

        image = ReadTGA(Buffer);

        return image;
    }
};