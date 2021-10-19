#include "partition.h"
#include "../utils/cstr.h"
#include "./DriveBaseSectors.h"
#include "../screen/rendering/BasicRenderer.h"

//For referencing disk
uint8_t NextMountable = 0x41;

//Null Setup
Partition::Partition(){
    //
}

//Propper Setup
void Partition::Init(GPT_PARTITION_ENTRY* Entry){
    //Stop Re-run
    prepared = true;

    //Set Entry
    GPTEntry = Entry;

    //Read format
    switch (GPTEntry->PartitionTypeGUID.GetGUIDType())
    {
    case UNSUSED_ENTRY:
        Format = UNKOWN_FORMAT;
        break;

    case BASIC_DATA_PARITITON:
        if(strcmp(PartitionMBR->FATName, "EXFAT", 5)){
            Format = EXFAT;
        }
        else if(strcmp(PartitionMBR->FATName, "FAT32", 5)){
            Format = FAT32;
        }
        else if(strcmp(PartitionMBR->FATName, "FAT16", 5)){
            Format = FAT16;
        }
        else if(strcmp(PartitionMBR->FATName, "FAT12", 5)){
            Format = FAT12;
        }
        else if(strcmp(PartitionMBR->FATName, "NTFS", 4)){
            Format = NTFS;
        }

        FLAG_READONLY_ = (GPTEntry->Attributes >> 60 == 1);
        FLAG_HIDDEN_ = (GPTEntry->Attributes >> 62 == 1);
        FLAG_AUTOMOUNT_ = (GPTEntry->Attributes >> 63 == 1);
        break;

    case LINUX_FILESYSTEM_DATA:
        Format = EXT;
        break;

    case APFS_CONTAINER:
        Format = APFS;
        break;
    
    case APFS_RECOVERY:
        Format = APFS;
        break;
    
    default:
        Format = UNKOWN_FORMAT;
        break;
    }

    //Mount Disk if can and AutoMount
    if(FLAG_AUTOMOUNT && FLAG_HIDDEN && NextMountable < 0x5A){
        Mounted_ = true;
        MountPoint_ = NextMountable++;
    }

    //Calculate Root Director Start
    RDSector_ = PartitionMBR->NumberOfSectorsBeforeStart + (2 * PartitionMBR->NumberOfSectorsPerFat) + PartitionMBR->SizeOfReservedSectors;
}