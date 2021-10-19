#include "systemManager.h"

#include "../screen/screenManager.h"
#include "../filesystem/fileManager.h"
#include "../paging/PageFrameAllocator.h"

//System Manager For OS
SystemManager* GSystemManager = new SystemManager();

SystemManager::SystemManager(){
    //Empty
}

SystemManager::~SystemManager(){
    //Empty
}

//Setup
void SystemManager::Initliaze(Framebuffer* fb){
    //FileSystem
    GlobalFileManager.Initialize();

    //Screen
    GScreenManager->framebuffer = fb;
    //GScreenManager->Intialize();
}

//Clocking
void SystemManager::Update(){
    
}