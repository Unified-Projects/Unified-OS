#include "BasicRenderer.h"
#include "../../memory/memory.h"
#include "../../userinput/mouse.h"

BasicRenderer* GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer* fb, PSF1_FONT* f){
    framebuffer = fb;
    psf1_font = f;
    CursorPosition = {0, 0};
}

void BasicRenderer::putPix(uint32_t x, uint32_t y, uint32_t color){
    *(uint32_t*)((uint64_t)framebuffer->BaseAddress + (x*4) + (y * framebuffer->PixelsPerScanLine *4)) = color;
}

uint32_t BasicRenderer::getPix(uint32_t x, uint32_t y){
    return *(uint32_t*)((uint64_t)framebuffer->BaseAddress + (x*4) + (y * framebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::DrawMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color){
    int xMax = 16;
    int yMax = 16;
    int differenceX = framebuffer->Width - position.x;
    int differenceY = framebuffer->Height - position.y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                MouseCursorBuffer[x + y * 16] = getPix(position.x + x, position.y + y);
                putPix(position.x + x, position.y + y, color);
                MouseCursorBufferAfter[x + y * 16] = getPix(position.x + x, position.y + y);
            }
        }
    }

    MouseDrawn = true;
}

void BasicRenderer::ClearMouseCursor(uint8_t* mouseCursor, Point position){
    if (!MouseDrawn) return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = framebuffer->Width - position.x;
    int differenceY = framebuffer->Height - position.y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                if (getPix(position.x + x, position.y + y) == MouseCursorBufferAfter[x + y *16]){
                    putPix(position.x + x, position.y + y, MouseCursorBuffer[x + y * 16]);
                }
            }
        }
    }
}

void BasicRenderer::Clear(uint32_t color){
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t bytesPerScanLine = framebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = framebuffer->Height;
    uint64_t fbSize = framebuffer->BufferSize;

    for(int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline++){
        uint64_t pixPtrBase = fbBase + (bytesPerScanLine * verticalScanline);
        for(uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanLine); pixPtr++){
            *pixPtr = color;
        }
    }

    this->CursorPosition = {0, 0};
}

void BasicRenderer::ClearChar(){

    if (CursorPosition.x == 0){
        CursorPosition.x = framebuffer->Width;
        CursorPosition.y -= 16;
        if (CursorPosition.y < 0) CursorPosition = {0, 0};
    }

    unsigned int xOff = CursorPosition.x;
    unsigned int yOff = CursorPosition.y;

    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff - 8; x < xOff; x++){
            *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = 0x00000000;
        }
    }

    CursorPosition.x -= 8;

    if (CursorPosition.x < 0){
        CursorPosition.x = framebuffer->Width;
        CursorPosition.y -= 16;
        if (CursorPosition.y < 0) CursorPosition = {0, 0};
    }
}

void BasicRenderer::Next(){
    CursorPosition = {0, CursorPosition.y += 16};
}

void BasicRenderer::putChar(unsigned int color, char chr, unsigned int xOff, unsigned int yOff){
    ClearMouseCursor(MousePointer, CursorPosition);
    
    while (CursorPosition.y + 16 >= framebuffer->Height)
    {
        ShiftUp();
    }

	unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddress;
    char* fontPtr = (char*)psf1_font->glyphBuffer + (chr * psf1_font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void BasicRenderer::putChar(char chr, unsigned int color){
    ClearMouseCursor(MousePointer, CursorPosition);
    
    while (CursorPosition.y + 16 >= framebuffer->Height)
    {
        ShiftUp();
    }
    
    putChar(color, chr, CursorPosition.x, CursorPosition.y);
    CursorPosition.x += 8;
    if(CursorPosition.x + 8 > framebuffer->Width){
        Next();
    }
}

void BasicRenderer::ShiftUp(){

    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddress;

    for(unsigned long r = 16; r < framebuffer->Height; r++){
        for (unsigned long c = 0; c < framebuffer->Width; c++){
            *(unsigned int*)(pixPtr + (c) + ((r - 16) * framebuffer->PixelsPerScanLine)) = *(unsigned int*)(pixPtr + c + (r * framebuffer->PixelsPerScanLine));
            *(unsigned int*)(pixPtr + c + (r * framebuffer->PixelsPerScanLine)) = 0x00000000;
        }
    }

    CursorPosition.y -= 16;
}

void BasicRenderer::PrintAtCursor(const char* str, unsigned int color){
    ClearMouseCursor(MousePointer, CursorPosition);
    
    while (CursorPosition.y + 16 >= framebuffer->Height)
    {
        ShiftUp();
    }

	char* chr = (char*)str;
	while(*chr != 0){
        if(*chr != '\n'){
            putChar(color, *chr, CursorPosition.x, CursorPosition.y);
            CursorPosition.x+=8;
            if(CursorPosition.x + 8 > framebuffer->Width){
                Next();
            }
        }
        else{
            Next();
        }
		chr++;
	}
}

void BasicRenderer::PrintAtPos(const char* str, unsigned int x, unsigned int y, unsigned int color){
    unsigned int backX;
	char* chr = (char*)str;
	while(*chr != 0){
        if(*chr != '\n'){
            putChar(color, *chr, x, 0);
		    x+=8;
            if(x + 8 > framebuffer->Width){
                x = backX;
                y += 16;
            }
        }
        else{
            x = backX;
            y += 16;
        }
		chr++;
	}
}