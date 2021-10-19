#pragma once

#include "../../Math/Math.h"
#include "../FrameBuffer.h"
#include "../../Font/Font.h"
#include <cstddef>
#include <stdint.h>


class BasicRenderer{
public:
    //System Information For Rendering
    Framebuffer* framebuffer;
    PSF1_FONT* psf1_font;

    //Cursors
    Point CursorPosition;
    bool MouseDrawn;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];

    //Init
    BasicRenderer(Framebuffer* fb, PSF1_FONT* f);

    //Shifs the screen up 1 row of text (16px)
    void ShiftUp();

    //Places one pixel onto the screen
    void putPix(uint32_t x, uint32_t y, uint32_t color);
    //get once pixel
    uint32_t getPix(uint32_t x, uint32_t y);

    //Put text on the screen
    void putChar(unsigned int color, char chr, unsigned int xOff, unsigned int yOff); //At position
    void putChar(char chr, unsigned int color = 0xffffffff); //At cursor

    //Printing entire strings onto the screen
    void PrintAtCursor(const char* str, unsigned int color = 0xffffffff); //At Cursor
    void PrintAtPos(const char* str, unsigned int x = 0, unsigned int y = 0, unsigned int color = 0xffffffff); //Offset from 0, 0

    //Removes last character on screen (Cursor only)
    void ClearChar();
    void Clear(uint32_t color = 0x00000000); //Clears the whole buffer

    //Move Cursor Down a line
    void Next();

    //Mouse Draw/Clearing
    void ClearMouseCursor(uint8_t* mouseCursor, Point position);
    void DrawMouseCursor(uint8_t* mouseCursor, Point position, uint32_t color);
};

//Global
extern BasicRenderer* GlobalRenderer;