#pragma once

#include "file.h"

namespace DIR{
    //What a directory is
    struct Directory{
        //Files in it
        FILE::File* Files;
        uint64_t FileSize = 0;

        //Possible other Directories
        Directory* Directories;
        uint64_t DirSize = 0;

        //Entry (At Cluster Position For Re referencing)
        DIR_DIRECTORY_ENTRY DirEntry;
        uint64_t SectorLocationStart;

        //Main Entry (Refernece)
        DIRECTORY_ENTRY DirectEntry;

        //Main Name
        char DirectoryName[64];

        //Hiding
        bool Hidden = false;
    };
}