#pragma once

#include "../ahci/port.h"

//For Comparing Attribute Data
enum FILE_ATTRIBUTES_MASK{
    Read_OnlyM = 0x01, //Mask with this and bit shift >> 0 and if == 1 its toggled
    HiddenM = 0x02, //Mask with this and bit shift >> 1 and if == 1 its toggled
    System_FileM = 0x04, //Mask with this and bit shift >> 2 and if == 1 its toggled
    Volume_LabelM = 0x08, //Mask with this and bit shift >> 3 and if == 1 its toggled
    Long_File_NameM = 0x0F, //Mask with this and bit shift >> 0 and if == 15 its toggled
    DirectoryM = 0x10, //Mask with this and bit shift >> 4 and if == 1 its toggled
    ArchiveM = 0x20 //Mask with this and bit shift >> 5 and if == 1 its toggled
};

//Main attribute Struct
struct FILE_ATTRIBUTE{
    //Bools
    bool Read_Only = false;
    bool Hidden = false;
    bool System_File = false;
    bool Volume_Label = false;
    bool Long_File_Name = false;
    bool Directory = false;
    bool Archive = false;

    //Setter
    void Set(uint8_t byte){
        Read_Only = ((byte&Read_OnlyM) >> 0 == 1);
        Hidden = ((byte&HiddenM) >> 1 == 1);
        System_File = ((byte&System_FileM) >> 2 == 1);
        Volume_Label = ((byte&Volume_LabelM) >> 3 == 1);
        Long_File_Name = ((byte&Long_File_NameM) >> 0 == 15);
        Directory = ((byte&DirectoryM) >> 4 == 1);
        Archive = ((byte&ArchiveM) >> 5 == 1);
    }

    //Getter
    uint8_t Get(){
        uint8_t Byte; //Should Probably Mask the data if it does not work
        Byte |= Read_Only;
        Byte |= Hidden << 1;
        Byte |= System_File << 2;
        Byte |= Volume_Label << 3;
        Byte |= Long_File_Name * 0xF;
        Byte |= Directory << 4;
        Byte |= Archive << 5;
        return Byte;
    }
};

//Implement Long File Names Additional 32B entries contain the file's long filename (LFN)
struct ROOT_DIRECTORY_ENTRY{
    uint8_t AllocationStatus; //Byte 0 //Status's (0x00 = unallocated) (0xe5 = deleted)
    char* FileName; //Bytes 1 - 10 //If Not using allocation status (We will add First Char)
    FILE_ATTRIBUTE FileAttributes; //Byte 11
    uint8_t Reserved; //Byte 12
    uint8_t FileCreationTime; //Byte 13 //Tenth of seconds //Not Used By DOS
    uint32_t CreationTime; //Byte 14 - 15 //(Hours, Minutes, Seconds) //Not Used By DOS
    uint16_t CreationDate; //Byte 16 - 17 //Not Used By DOS
    uint16_t AccessDate; //Byte 18 - 19 //Not Used By DOS
    uint16_t High2BytesOfAddressOfFirstCluster; //Bytes 20 - 21 //0 for FAT12/FAT16
    uint16_t ModifiedTime; //Bytes 22 - 23 //(Hours, Minutes, Seconds)
    uint16_t ModifiedDate; //Bytes 24 - 25
    uint16_t Low2BytesOfAddressOfFirstCluster; //Bytes 26 - 27
    uint32_t FileSize; //Bytes 28 - 31 //0 For Directories (FLIP)
};

struct LFN_DRIECTORY_ENTRY{
    uint8_t SequenceNumberAndAllocationStatus; //Byte 0 //Status's (0x00 = unallocated) (0xe5 = deleted)
    char* FileName; //Bytes (1 - 10) (14 -25) (28 - 31)
    FILE_ATTRIBUTE FileAttributes; //Byte 11
    uint8_t Reserved; //Byte 12
    uint8_t Checksum; //Byte 13
    uint16_t Reserved2; //Byte 26 - 27
};

struct DOT_ENTRY{
    uint8_t Dot; //Byte 0 First DOT (Directory Start)
    uint8_t Dot2; //Bytes 1 Second DOT (Back a directory)
    uint8_t Unsused[9]; //Bytes 2 - 10;
    FILE_ATTRIBUTE FileAttributes; //Byte 11
    uint8_t Reserved; //Byte 12
    uint8_t FileCreationTime; //Byte 13 //Tenth of seconds //Not Used By DOS
    uint32_t CreationTime; //Byte 14 - 15 //(Hours, Minutes, Seconds) //Not Used By DOS
    uint16_t CreationDate; //Byte 16 - 17 //Not Used By DOS
    uint16_t AccessDate; //Byte 18 - 19 //Not Used By DOS
    uint16_t High2BytesOfAddressOfFirstCluster; //Bytes 20 - 21 //0 for FAT12/FAT16
    uint16_t ModifiedTime; //Bytes 22 - 23 //(Hours, Minutes, Seconds)
    uint16_t ModifiedDate; //Bytes 24 - 25
    uint16_t Low2BytesOfAddressOfFirstCluster; //Bytes 26 - 27
    uint32_t FileSize; //Bytes 28 - 31 //0 For Directories (FLIP)
};


struct DIRECTORY_ENTRY{
    ROOT_DIRECTORY_ENTRY RDE;

    LFN_DRIECTORY_ENTRY* LFNE;  //Last == First (They Stack, Its just how it is in the actual
    uint16_t LFNEntries = 0;                      // data of the drive the last is first so I need to flip)

    bool Valid = false;
};

struct DIR_DIRECTORY_ENTRY{
    DOT_ENTRY Single;
    DOT_ENTRY Back;

    bool Valid = false;
};