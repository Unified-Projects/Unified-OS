#pragma once

#include <stdint.h>
#include "../../screen/FrameBuffer.h"
#include "../../Math/Math.h"

struct WindowHeader{
    //Title
    const char* Title;
    
    //Icon
    void* IconBuffer;

    //Settings
    bool Resizeable;

    //Positional
    Point Position;
    
    //Sizing
    Point Size;
};


class Window{
private:
    //For the window to render too (Including title)
    Framebuffer framebuffer;
public:

    //Just the window buffer for content (Public so application can use it)
    Framebuffer windowBuffer;

    //For window details
    WindowHeader header;
    
    //Main intializers
    void Start();
    void End();

    void PutFrameBuffer(Framebuffer fb);
};