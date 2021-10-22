#pragma once

#include "./DirectoryEntries.h"
#include "./DiskReading.h"
#include "drive.h"

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

namespace FILE{
    struct File
    {
    private:
        //Actuall Data Entry
        FAT12Entry F12Entry;
        FAT16Entry F16Entry;
        FAT32Entry F32Entry;
        FAT64Entry F64Entry;

    public:
        //File Info
        char* FileName;
        uint16_t FileNameSize = 0;
        char* Extention;
        uint8_t ExtentionSize = 0;

        //If a file and how much data to read
        uint64_t FileSize;

        //Drive Position
        DIRECTORY_ENTRY DirEntry;

        //Empty Data unless read into
        void* data;

        //Frees data from buffer
        void FreeData();

        //Reads data to buffer
        void ReadData();

        //Find Data
        File(); //NULL VALUE
        File(DIRECTORY_ENTRY entry); //Will Not COMPLETE SETUP
        File(DIRECTORY_ENTRY entry, Drive* drive, Partition* partition); //Setup

    private:
        //For data read/clear
        uint64_t bufferPageCount = 0;
        uint64_t sectorCount = 0;

        //Hiding
        bool Hidden = false;

        //Valid
        bool Exists = false;

    private:
        //Setup
        bool Setup = false;

        //Drive Reference
        Drive* DrivePtr = NULL;
        Partition* PartitionPtr = NULL;
    };
}