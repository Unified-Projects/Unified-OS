#pragma once

#include <stdint.h>
#include "FrameBuffer.h"

class ScreenManager{
private:
public:
    Framebuffer* framebuffer;

    ScreenManager();
    ~ScreenManager();

    void Intialize();
};

//Main Screen Setup
extern ScreenManager* GScreenManager;