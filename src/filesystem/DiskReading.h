#pragma once
#include "drive.h"
#include "directory.h"

//Fat Entry Reading
struct FAT64Entry{ //NTFS
    uint64_t* Clusters;
};
struct FAT32Entry{
    uint64_t* Clusters;
};
struct FAT16Entry{
    uint64_t* Clusters;
};
struct FAT12Entry{
    uint64_t* Clusters;
};


//RDE Getter
ROOT_DIRECTORY_ENTRY GetRDE(void* buffer, uint64_t EntryOffset);

//LFN Getter
LFN_DRIECTORY_ENTRY GetLFNDE(void* buffer, uint64_t EntryOffset);

//Attribute Checker
bool isLFN(void* buffer, uint64_t EntryOffset);

//Entry for a file or a folder getter
DIRECTORY_ENTRY RootGetDirectoryEntry(void* buffer, uint64_t EntryOffset, uint32_t rootStart);

//Separate Folder Entries
DOT_ENTRY GetFolerInEntry();
ROOT_DIRECTORY_ENTRY GetFileInDirectory();