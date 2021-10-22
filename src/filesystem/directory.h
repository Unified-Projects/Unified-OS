#pragma once

#include "file.h"

namespace DIR{
    //What a directory is
    struct Directory{
    private:
        //Actuall Data Entry
        FAT12Entry F12Entry;
        FAT16Entry F16Entry;
        FAT32Entry F32Entry;
        FAT64Entry F64Entry;

        uint64_t Entries = 0;

    public:

        Directory();
        Directory(uint64_t FirstCluster, Drive* drive, Partition* partition);

        //Files in it
        FILE::File* Files;
        uint64_t FileSize = 0;

        //Entries within directory
        DIRECTORY_ENTRY* DirEntries;
        uint64_t SizeOfEntries = 0;

        //Entry (At Cluster Position For Re referencing)
        DIR_DIRECTORY_ENTRY DirEntry;
        uint64_t SectorLocationStart;

        //Main Entry (Refernece)
        DIRECTORY_ENTRY DirectEntry;

        //Main Name
        char DirectoryName[64];

        //Hiding
        bool Hidden = false;
        
    private:
        //Setup
        bool Setup = false;

        //Drive Reference
        Drive* DrivePtr = NULL;
        Partition* PartitionPtr = NULL;
    };
}