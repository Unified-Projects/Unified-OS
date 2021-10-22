#pragma once

#include "drive.h"
#include "directory.h"
#include "DiskReading.h"
#include "../ahci/port.h"

class FileManager{
private:
    //Stops reinitialization
    bool Initialized;
public:

    //Reads all the sytems data and stores it to be accessed
    void Initialize();

    FILE::File FindFile(const char* path);
};

//Globaliser
extern FileManager GlobalFileManager;