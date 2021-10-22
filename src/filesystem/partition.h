#pragma once

#include "../ahci/port.h"
#include "DriveBaseSectors.h"

enum PARTITION_FORMAT{
    NOT_PRESENT = -1,
    UNKOWN_FORMAT = 0,

    FAT12 = 1,
    FAT16 = 2,
    FAT32 = 3,
    EXFAT = 4,

    NTFS = 5,
    
    EXT = 6,

    APFS = 7,
};

extern uint8_t NextMountable;

//Partition
class Partition{
private:
    bool prepared = false;

//Read Only
    //Basic Data Parition
    bool FLAG_READONLY_ = false; //Bit 60
    bool FLAG_HIDDEN_ = false; //Bit 62
    bool FLAG_AUTOMOUNT_ = false; //Bit 63

    bool Mounted_ = false;
    uint8_t MountPoint_ = 0x00;

    //Root Dir Sector Start
    uint64_t RDSector_ = 0x00;

public:
    //File format
    PARTITION_FORMAT Format = UNKOWN_FORMAT;

    //Buffer for boot sector
    MBR* PartitionMBR;
    GPT_PARTITION_ENTRY* GPTEntry;
    
    //Setup
    Partition();
    void Init(GPT_PARTITION_ENTRY* GPTEntry);

//Read Only
    //Basic Data Parition
    const bool &FLAG_READONLY = FLAG_READONLY_; //Bit 60
    const bool &FLAG_HIDDEN = FLAG_HIDDEN_; //Bit 62
    const bool &FLAG_AUTOMOUNT = FLAG_AUTOMOUNT_; //Bit 63

    const bool &Mounted = Mounted_;
    const uint8_t &MountPoint = MountPoint_;

    //Root Dir Sector Start
    const uint64_t &RDSector = RDSector_;
};
