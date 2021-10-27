#include "tga.h"

#include "../../memory/heap.h"
#include "../../filesystem/ByteInteraction.h"

TGA_Image TGA::ReadTGA(){
    File->ReadData();

    uint8_t* Buffer = (uint8_t*)File->data;

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
                    ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = (Get4Byte(Buffer, 0x00) >> 8) | (0xFF << 24);
                    offsetBuffer(Buffer, 3);
                }
            }
            break;
        
        case 32: //AA RR GG BB
            for(int y = 0; y < header.height; y++){
                for(int x = 0; x < header.width; x++){
                    ImageDataBuffer[(((header.height - 1) - y) * header.width) + x] = (Get4Byte(Buffer, 0x00) >> 8) | (GetByte(Buffer, 3) << 24);
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

    File->FreeData();

    return image;
}

TGA::TGA(FILE::File* file){
    File = file;
}

TGA::~TGA(){

}

TGA_Image TGA::GetImage(){
    TGA_Image image;

    image = ReadTGA();

    return image;
}