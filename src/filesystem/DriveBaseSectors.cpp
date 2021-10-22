#include "DriveBaseSectors.h"
#include "ByteInteraction.h"
#include "../memory/heap.h"

//GUID Get Type
GUIDTypes GUID::GetGUIDType(){
    GUIDTypes type;

    //Check types
    if(*this == GUID{0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00000000}){
        type = UNSUSED_ENTRY;
    }
    else if(*this == GUID{0x024DEE41, 0x33E7, 0x11D3, 0x9D69, 0x0008, 0xC781F39F}){
        type = MBR_PARTITION_SCHEME;
    }
    else if(*this == GUID{0xC12A7328, 0xF81F, 0x11D2, 0xBA4B, 0x00A0, 0xC93EC93B}){
        type = EFI_SYSTEM_PARTITION;
    }
    else if(*this == GUID{0x21686148, 0x6449, 0x6E6F, 0x744E, 0x6565, 0x64454649}){
        type = BIOS_BOOT_PARITITON;
    }
    else if(*this == GUID{0xD3BFE2DE, 0x3DAF, 0x11DF, 0xBA40, 0xE3A5, 0x56D89593}){
        type = INTEL_FAST_FLASH_PARTITION;
    }
    else if(*this == GUID{0xE3C9E316, 0x0B5C, 0x4DB8, 0x817D, 0xF92D, 0xF00215AE}){
        type = MICROSOFT_RESERVED_PARITITON;
    }
    else if(*this == GUID{0xEBD0A0A2, 0xB9E5, 0x4433, 0x87C0, 0x68B6, 0xB72699C7}){
        type = BASIC_DATA_PARITITON;
    }
    else if(*this == GUID{0xE75CAF8F, 0xF680, 0x4CEE, 0xAFA3, 0xB001, 0xE56EFC2D}){
        type = STORAGE_SPACES_PARTITION;
    }
    else if(*this == GUID{0x0FC63DAF, 0x8483, 0x4772, 0x8E79, 0x3D69, 0xD8477DE4}){
        type = LINUX_FILESYSTEM_DATA;
    }
    else if(*this == GUID{0x4F68BCE3, 0xE8CD, 0x4DB1, 0x96E7, 0xFBCA, 0xF984B709}){
        type = LINUX_BOOT;
    }
    else if(*this == GUID{0xBC13C2FF, 0x59E6, 0x4262, 0xA352, 0xB275, 0xFD6F7172}){
        type = LINUX_ROOT;
    }
    else if(*this == GUID{0x0657FD6D, 0xA4AB, 0x43C4, 0x84E5, 0x0933, 0xC84B4F4F}){
        type = LINUX_SWAP;
    }
    else if(*this == GUID{0x933AC7E1, 0x2EB4, 0x4F13, 0xB844, 0x0E14, 0xE2AEF915}){
        type = LINUX_HOME;
    }
    else if(*this == GUID{0x7C3457EF, 0x0000, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC}){
        type = APFS_CONTAINER;
    }
    else if(*this == GUID{0x52637672, 0x7900, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC}){
        type = APFS_RECOVERY;
    }
    else{
        type = UNSUSED_ENTRY;
    }

    return type;
}

//set a GUID to a type
void GUID::SetGUID(GUIDTypes type){
    //Check types
    if(type == UNSUSED_ENTRY){
        *this = GUID{0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00000000};
    }
    else if(type == MBR_PARTITION_SCHEME){
        *this = GUID{0x024DEE41, 0x33E7, 0x11D3, 0x9D69, 0x0008, 0xC781F39F};
    }
    else if(type == EFI_SYSTEM_PARTITION){
        *this = GUID{0xC12A7328, 0xF81F, 0x11D2, 0xBA4B, 0x00A0, 0xC93EC93B};
    }
    else if(type == BIOS_BOOT_PARITITON){
        *this = GUID{0x21686148, 0x6449, 0x6E6F, 0x744E, 0x6565, 0x64454649};
    }
    else if(type == INTEL_FAST_FLASH_PARTITION){
        *this = GUID{0xD3BFE2DE, 0x3DAF, 0x11DF, 0xBA40, 0xE3A5, 0x56D89593};
    }
    else if(type == MICROSOFT_RESERVED_PARITITON){
        *this = GUID{0xE3C9E316, 0x0B5C, 0x4DB8, 0x817D, 0xF92D, 0xF00215AE};
    }
    else if(type == BASIC_DATA_PARITITON){
        *this = GUID{0xEBD0A0A2, 0xB9E5, 0x4433, 0x87C0, 0x68B6, 0xB72699C7};
    }
    else if(type == STORAGE_SPACES_PARTITION){
        *this = GUID{0xE75CAF8F, 0xF680, 0x4CEE, 0xAFA3, 0xB001, 0xE56EFC2D};
    }
    else if(type == LINUX_FILESYSTEM_DATA){
        *this = GUID{0x0FC63DAF, 0x8483, 0x4772, 0x8E79, 0x3D69, 0xD8477DE4};
    }
    else if(type == LINUX_BOOT){
        *this = GUID{0x4F68BCE3, 0xE8CD, 0x4DB1, 0x96E7, 0xFBCA, 0xF984B709};
    }
    else if(type == LINUX_ROOT){
        *this = GUID{0xBC13C2FF, 0x59E6, 0x4262, 0xA352, 0xB275, 0xFD6F7172};
    }
    else if(type == LINUX_SWAP){
        *this = GUID{0x0657FD6D, 0xA4AB, 0x43C4, 0x84E5, 0x0933, 0xC84B4F4F};
    }
    else if(type == LINUX_HOME){
        *this = GUID{0x933AC7E1, 0x2EB4, 0x4F13, 0xB844, 0x0E14, 0xE2AEF915};
    }
    else if(type == APFS_CONTAINER){
        *this = GUID{0x7C3457EF, 0x0000, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC};
    }
    else if(type == APFS_RECOVERY){
        *this = GUID{0x52637672, 0x7900, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC};
    }
    else{
        *this = GUID{0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00000000};
    }
}

//Opeartor For Comparisons
bool operator ==(GUID l, GUID r){
    if(l.Entry1 != r.Entry1){
        return false;
    }
    if(l.Entry2 != r.Entry2){
        return false;
    }
    if(l.Entry3 != r.Entry3){
        return false;
    }
    if(l.Entry4 != r.Entry4){
        return false;
    }
    if(l.Entry5H != r.Entry5H){
        return false;
    }
    if(l.Entry5L != r.Entry5L){
        return false;
    }  

    return true;
}

//Opeartor For Comparisons
bool operator ==(GUID l, GUIDTypes r){
    if(l.GetGUIDType() == r){
        return true;
    }
    return false;
}

//Gets a Partition Entrty From A MBR
MBR_PARTITION_ENTRY ReadMBRPartition(void* Buffer, uint16_t Offset){
    //Value
    MBR_PARTITION_ENTRY PEntry;

    //Parititon State
    PEntry.State = GetByte(Buffer, Offset + 0x00);

    //Partition Head (High Byte)
    PEntry.BeginningOfPartitionHead = GetByte(Buffer, Offset + 0x01);

    //Cylinder Sector (High Byte)
    PEntry.BeginningOfPartitionCylinderSector = Get2Byte(Buffer, Offset + 0x02);

    //Parition Type (Refer to the header file)
    PEntry.TypeOfPartition = GetByte(Buffer, Offset + 0x04);

    //Partition Head (Low Byte)
    PEntry.EndOfPartitionHead = GetByte(Buffer, Offset + 0x05);

    //Cylinder Sector (Low Byte)
    PEntry.EndOfParitionCylinderSector = Get2Byte(Buffer, Offset + 0x06);

    //Sectors between MBR and Storage Sector
    PEntry.NumberOfSectorsBetweenTheMBRAndFirstSector = Flip4Byte(Get4Byte(Buffer, Offset + 0x08));

    //Sector Count
    PEntry.NumberOfSectors = Flip4Byte(Get4Byte(Buffer, Offset + 0x0C));
    
    //Return Value
    return PEntry;
}

//Reads the Sectors Data into a MBR entry
MBR* ReadMBR(void* Sector, MBR* Location){
    //Retrun Value
    //Location = new MBR{};

    Location = new MBR{};

    //JumpCode/Op
    Location->JumpCodeAndNOP = Get4Byte(Sector, 0) >> 8;

    //OEMName
    for(int i = 0; i < 8; i++){
        Location->OEMName[i] = GetByte(Sector, 0x03 + i);
    }

    //BPS
    Location->BytesPerSector = Flip2Byte(Get2Byte(Sector, 0x0B));

    //Sectors Per Cluster
    Location->SectorsPerCluster = GetByte(Sector, 0x0D);

    //Number of Reserved Sectors (Before The First FAT)
    Location->SizeOfReservedSectors = Flip2Byte(Get2Byte(Sector, 0x0E));

    //How many FATS (usually 2)
    Location->NumberOfFATs = GetByte(Sector, 0x10);

    //Root directory Size FAT12/FAT16 only
    Location->MaxNumberFilesInRootDir = Flip2Byte(Get2Byte(Sector, 0x11));
    
    //Sectors in a partition (Not for FAT32)
    Location->NumberOfSectorsInPartitionNFAT32 = Flip2Byte(Get2Byte(Sector, 0x13));
    
    //Type of Disk
    Location->MediaType = GetByte(Sector, 0x15); //0xf0 is removable, 0xf8 is hard disk
    
    //Size of FAT's FAT12/FAT16 only
    Location->SizeOfFATInSectors = Flip2Byte(Get2Byte(Sector, 0x16));
    
    //SPT
    Location->SectorsPerTrack = Get2Byte(Sector, 0x18);
    
    //Head Count
    Location->NumberOfHeads = Get2Byte(Sector, 0x1A);
    
    //Sectors before first data sector
    Location->NumberOfSectorsBeforeStart = Flip4Byte(Get4Byte(Sector, 0x1C));
    
    //Total Number Of Sectors in Partition
    Location->NumberOfSectorsInPartition = Flip4Byte(Get4Byte(Sector, 0x20));
    
    //Number of sectors in a File Allocation Table
    Location->NumberOfSectorsPerFat = Flip4Byte(Get4Byte(Sector, 0x24));
    
    //Flags
    //(Bits 0-4 Indicate FAT Copy) 
    //(Bit 7 Indicates FAT Mirroring) 
    //(If FATMirroring is Disabled, the FAT Information is onlywritten to the copy indicated by bits 0-4)
    Location->Flags = Get2Byte(Sector, 0x28);
    
    //FAT File Version
    //(Hight Byte = Major) (Low Byte = Minor)
    Location->FATVersion = Get2Byte(Sector, 0x2A);
    
    //Cluster Number of the Root Directory
    Location->ClusterNumberOfRootDirStart = Flip4Byte(Get4Byte(Sector, 0x2C));
    
    //FileSystem Information Sector
    Location->SectorNumberOfFileSystemInformationSector = Flip2Byte(Get2Byte(Sector, 0x30));
    
    //BackupBootSector
    Location->SectorNumberOfBackupBootSector = Flip2Byte(Get2Byte(Sector, 0x32));
    
    //Bytes 0x34 - 0x3F Reserved
    
    //Drive Number For Partition (0x41 usually unsused)
    Location->LogicalDriveNumberOfPartition = Get2Byte(Sector, 0x40); //Bytes 0x40 - 0x41 
    
    //Extended Signature (0x29)
    Location->ExtendedSignature = GetByte(Sector, 0x42); //Bytes 0x42 //
    
    //Partition SN
    Location->SerialNumberOfPartiton = Get4Byte(Sector, 0x43); //Bytes 0x43 - 0x46
    
    //Volume Name
    for(int i = 0; i < 11; i++){
        Location->VolumeNameOfPartition[i] = GetByte(Sector, 0x47 + i);
    }
    
    //FAT Name
    for(int i = 0; i < 8; i++){
        Location->FATName[i] = GetByte(Sector, 0x52 + i);
    }

    //Partition Tables
    Location->Entry1 = ReadMBRPartition(Sector, 0x1BE);
    Location->Entry2 = ReadMBRPartition(Sector, 0x1CE);
    Location->Entry3 = ReadMBRPartition(Sector, 0x1DE);
    Location->Entry4 = ReadMBRPartition(Sector, 0x1EE);

    //End (0x55AA)
    Location->BootRecordSignature = Get2Byte(Sector, 0x1F3);

    //Return
    //return val;
    return Location;
}   

//Guid Reader;
GUID ReadGUID(void* buffer, uint64_t offset){
    //Set
    GUID guid = GUID{};

    //First Section
    guid.Entry1 = Flip4Byte(Get4Byte(buffer, offset));

    //Second
    guid.Entry2 = Flip2Byte(Get2Byte(buffer, offset + 0x04));

    //Third
    guid.Entry3 = Flip2Byte(Get2Byte(buffer, offset + 0x06));

    //Fourth
    guid.Entry4 = Get2Byte(buffer, offset + 0x08);

    //Fith
    guid.Entry5H = Get2Byte(buffer, offset + 0x0A);
    guid.Entry5L = Get4Byte(buffer, offset + 0x0C);

    //Return
    return guid;
}

//Read Main GPTHeader
GPT* ReadGPT(void* Sector, GPT* Location){
    //Initiate
    Location = new GPT{};

    //Signature (Same always)("EFI PART")
    for(int i = 0; i < 8; i++){
        Location->Signature[i] = GetByte(Sector, 0x00 + i);
    }

    //Revision Number
    Location->Revision = Flip4Byte(Get4Byte(Sector, 0x08));

    //Header Size
    Location->HeaderSize = Flip4Byte(Get4Byte(Sector, 0x0C));

    //Checksum
    Location->CRC32Checksum = Get4Byte(Sector, 0x10);

    //Header Block (1)
    Location->LBAOfHeader = Flip8Byte(Get8Byte(Sector, 0x18));

    //Copy (End of Disk)
    Location->LBAOfAlternateHeader = Flip8Byte(Get8Byte(Sector, 0x20));

    //First Data Block
    Location->FirstUsableBlock = Flip8Byte(Get8Byte(Sector, 0x28));

    //Last Data Block
    Location->LastUsableBlock = Flip8Byte(Get8Byte(Sector, 0x30));

    //GUID
    Location->GUIDMain = ReadGUID(Sector, 0x38);

    //Starting LBA (Normally 2)
    Location->StartingLBAofGUIDPartitionEntries = Flip8Byte(Get8Byte(Sector, 0x48));

    //Partition Max Entries (128)
    Location->NumberOfPartitionEntries = Flip4Byte(Get4Byte(Sector, 0x50));

    //Size of an Entry (128) (Must Be a value of 128*2^n)
    Location->SizeOfParitionEntries = Flip4Byte(Get4Byte(Sector, 0x54));

    //CRC32 Of Entry
    Location->CRC32OfParitionEntry = Flip4Byte(Get4Byte(Sector, 0x58));

    //Return Value
    return Location;
}

//Reading GPT Partitions
GPT* ReadGPTParitions(AHCI::Port* port, GPT* Location){
    //Parse Entries
    Location->Entries = new GPT_PARTITION_ENTRY[Location->NumberOfPartitionEntries];

    //Loop Over Sectors
    for(int s = 0; s < Location->NumberOfPartitionEntries / 4; s++){
        //Read Sector
        port->Read(s + 2, 1, port->buffer);
        
        //Loop over entries
        for(int e = 0; e < 4; e++){
            //Calculate Sector Offset
            uint64_t offset = Location->SizeOfParitionEntries * e;

            //Check if there is a partition Entry
            if(Get8Byte(port->buffer, 0x00 + offset) != 0x00 && Get8Byte(port->buffer, 0x08 + offset) != 0x00){
                //Show its not empty
                Location->Entries[s*4 + e].Empty = false;

                //Increment Count of entries (For Looping Over Purposes (Some Efficiency))
                Location->EntryCount++;

                //Partition Type
                Location->Entries[s*4 + e].PartitionTypeGUID = ReadGUID(port->buffer, offset + 0x00);

                //GUID
                Location->Entries[s*4 + e].PartitionGUID = ReadGUID(port->buffer, offset + 0x10);

                //First Block
                Location->Entries[s*4 + e].StartingLBA = Flip8Byte(Get8Byte(port->buffer, offset + 0x20));

                //Last Block
                Location->Entries[s*4 + e].EndingLBA = Flip8Byte(Get8Byte(port->buffer, offset + 0x28));

                //Any Attributes
                Location->Entries[s*4 + e].Attributes = Get8Byte(port->buffer, offset + 0x30);

                //Name
                uint8_t index = 0;
                bool Skip = false;
                for(int i = 0; i < 72; i++){
                    if(!Skip){
                        Location->Entries[s*4 + e].PartitionName[index++] = GetByte(port->buffer, offset + 0x38 + i);
                    }
                    Skip = !Skip;
                }
            }
        }
    }

    //Returns
    return Location;
}