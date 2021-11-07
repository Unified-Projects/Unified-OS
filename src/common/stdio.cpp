#include <common/stdio.h>
#include <boot/bootinfo.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Boot;

uint32_t CursorX;
uint32_t CursorY;

void UnifiedOS::putPix(uint32_t x, uint32_t y, uint32_t color){
    *(uint32_t*)((uint64_t)__BOOT__BootContext__->framebuffer->BaseAddress + (x*4) + (y * __BOOT__BootContext__->framebuffer->PixelsPerScanLine *4)) = color;
}

uint32_t UnifiedOS::getPix(uint32_t x, uint32_t y){
    return *(uint32_t*)((uint64_t)__BOOT__BootContext__->framebuffer->BaseAddress + (x*4) + (y * __BOOT__BootContext__->framebuffer->PixelsPerScanLine * 4));
}

void UnifiedOS::Clear(uint32_t color){
    uint64_t fbBase = (uint64_t)__BOOT__BootContext__->framebuffer->BaseAddress;
    uint64_t bytesPerScanLine = __BOOT__BootContext__->framebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = __BOOT__BootContext__->framebuffer->Height;
    uint64_t fbSize = __BOOT__BootContext__->framebuffer->BufferSize;

    for(int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline++){
        uint64_t pixPtrBase = fbBase + (bytesPerScanLine * verticalScanline);
        for(uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanLine); pixPtr++){
            *pixPtr = color;
        }
    }

    CursorX = 0;
    CursorY = 0;
}

void UnifiedOS::ShiftUp(){

    unsigned int* pixPtr = (unsigned int*)__BOOT__BootContext__->framebuffer->BaseAddress;

    for(unsigned long r = 16; r < __BOOT__BootContext__->framebuffer->Height; r++){
        for (unsigned long c = 0; c < __BOOT__BootContext__->framebuffer->Width; c++){
            *(unsigned int*)(pixPtr + (c) + ((r - 16) * __BOOT__BootContext__->framebuffer->PixelsPerScanLine)) = *(unsigned int*)(pixPtr + c + (r * __BOOT__BootContext__->framebuffer->PixelsPerScanLine));
            *(unsigned int*)(pixPtr + c + (r * __BOOT__BootContext__->framebuffer->PixelsPerScanLine)) = 0x00000000;
        }
    }

    CursorY -= 16;
}

void UnifiedOS::Next(){
    CursorX = 0;
    CursorY += 16;
}

void UnifiedOS::putChar(unsigned int color, char chr, unsigned int xOff, unsigned int yOff){
    while (CursorY + 16 >= __BOOT__BootContext__->framebuffer->Height)
    {
        ShiftUp();
    }

	unsigned int* pixPtr = (unsigned int*)__BOOT__BootContext__->framebuffer->BaseAddress;
    char* fontPtr = (char*)__BOOT__BootContext__->psf1_font->glyphBuffer + (chr * __BOOT__BootContext__->psf1_font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                *(unsigned int*)(pixPtr + x + (y * __BOOT__BootContext__->framebuffer->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void UnifiedOS::putChar(char chr, unsigned int color){
    while (CursorY + 16 >= __BOOT__BootContext__->framebuffer->Height)
    {
        ShiftUp();
    }
    
    putChar(color, chr, CursorX, CursorY);
    CursorX += 8;
    if(CursorX + 8 > __BOOT__BootContext__->framebuffer->Width){
        Next();
    }
}

void UnifiedOS::printf(const char* str){
    while (CursorY + 16 >= __BOOT__BootContext__->framebuffer->Height)
    {
        ShiftUp();
    }

	char* chr = (char*)str;
	while(*chr != 0){
        if(*chr != '\n'){
            putChar(0xFFFFFFFF, *chr, CursorX, CursorY);
            CursorX+=8;
            if(CursorX + 8 > __BOOT__BootContext__->framebuffer->Width){
                Next();
            }
        }
        else{
            Next();
        }
		chr++;
	}
}

// void printfHex(uint8_t val){
//     char* ret = "0x  ";
//     char* hex = "0123456789ABCDEF";
//     ret[2] = hex[(val >> 4) & 0xF];
//     ret[3] = hex[val & 0xF];
//     printf(ret);
// }
