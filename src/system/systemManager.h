#pragma once

#include "../screen/FrameBuffer.h"

class SystemManager{
private:
    bool Initialized;

public:
    SystemManager();
    ~SystemManager();

    void Initliaze(Framebuffer* fb);

    void Update();
};

//Main System
extern SystemManager* GSystemManager;