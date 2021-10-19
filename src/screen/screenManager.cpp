#include "screenManager.h"
#include "../memory/memory.h"

//Global Screen
ScreenManager* GScreenManager = new ScreenManager();

//Empty Setup
ScreenManager::ScreenManager(){

}

//Empty Deconstructor
ScreenManager::~ScreenManager(){

}

//Clear Screen
void ScreenManager::Intialize(){
    memset(framebuffer->BaseAddress, 0x00, framebuffer->BufferSize);
}