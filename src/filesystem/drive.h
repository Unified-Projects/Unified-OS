#pragma once

#include "../ahci/port.h"
#include "./DriveBaseSectors.h"
#include "./partition.h"

//BootTable Type
enum DRIVE_TYPE{
    UNKOWN_DRIVE_TYPE = 0,
    MASTER_BOOT_RECORD = 1,
    GUID_PARTITION_TABLE = 2,
};

//Main Drive
class Drive{
private:
public:
    //Type (Starts off as unknown)
    DRIVE_TYPE DriveType = UNKOWN_DRIVE_TYPE;

    //Partitons
    Partition* Partitions;

    //The Main Port To Read From
    AHCI::Port* port;

    //Main MBR
    MBR* DriveMBR;

    //Main GPT
    GPT* DriveGPT;

    //Init
    Drive(AHCI::Port* portPtr);
};  

//Global Drives
extern Drive* Drives[];
extern uint8_t DriveCount;