#pragma once

#include <stdint.h>
#include "../ahci/port.h"

//Parition Cylender/Sector Encoding
/*
15 14 13 12 11 10 9 8	   7 6                    5 4 3 2 1 0
Cylinder Bits 7 to 0	   Cylinder Bits 9 + 8	  Sector Bits 5 to 0
*/

//Parition Types
/*
Value	    Description
00h	        Unknown or Nothing
01h	        12-bit FAT
04h	        16-bit FAT (Partition Smaller than 32MB)
05h	        Extended MS-DOS Partition
06h	        16-bit FAT (Partition Larger than 32MB)
0Bh	        32-bit FAT (Partition Up to 2048GB)
0Ch	        Same as 0BH, but uses LBA1 13h Extensions
0Eh	        Same as 06H, but uses LBA1 13h Extensions
0Fh	        Same as 05H, but uses LBA1 13h Extensions
*/

struct MBR_PARTITION_ENTRY{ //16 Bytes
    uint8_t State; //0x00
    uint8_t BeginningOfPartitionHead; //0x01
    uint16_t BeginningOfPartitionCylinderSector; //0x02 - 0x03
    uint8_t TypeOfPartition; //0x04
    uint8_t EndOfPartitionHead; //0x05
    uint16_t EndOfParitionCylinderSector; //0x06 - 0x07
    uint32_t NumberOfSectorsBetweenTheMBRAndFirstSector; //0x08 - 0x0B
    uint32_t NumberOfSectors; //0x0C - 0x0F
};

struct MBR{
    //Header
    uint32_t JumpCodeAndNOP; //0x00
    char OEMName[8]; //Bytes 0x03 - 0x0A
    uint16_t BytesPerSector; //Bytes 0x0B - 0x0C
    uint8_t SectorsPerCluster; //Byte 0x0D
    uint16_t SizeOfReservedSectors; //Bytes 0x0E - 0x0F
    uint8_t NumberOfFATs; //Byte 0x10 //Usually 2
    uint16_t MaxNumberFilesInRootDir; //Bytes 0x11 - 0x12 //FAT12/FAT16; 0 for FAT32
    uint16_t NumberOfSectorsInPartitionNFAT32; //Bytes 0x13 - 0x14 //0 for FAT32
    uint8_t MediaType; //Byte 0x15 //0xf0 is removable, 0xf8 is hard disk
    uint16_t SizeOfFATInSectors; //Bytes 0x16 - 0x17 //FAT12/FAT16; 0 for FAT32
    uint16_t SectorsPerTrack; //Bytes 0x18 - 0x19
    uint16_t NumberOfHeads; //Bytes 0x1A - 0x1B
    uint32_t NumberOfSectorsBeforeStart; //Bytes 0x1C - 0x1F
    uint32_t NumberOfSectorsInPartition; //Bytes 0x20 - 0x23;
    uint32_t NumberOfSectorsPerFat; //Bytes 0x24 - 0x27; //FAT32
    uint16_t Flags; //Bytes 0x28 - 0x29; //(Bits 0-4 Indicate FAT Copy) (Bit 7 Indicates FAT Mirroring) (If FATMirroring is Disabled, the FAT Information is onlywritten to the copy indicated by bits 0-4)
    uint16_t FATVersion; //Bytes 0x2A - 0x2B; (Hight Byte = Major) (Low Byte = Minor)
    uint32_t ClusterNumberOfRootDirStart; //Bytes 0x2C - 0x2F
    uint16_t SectorNumberOfFileSystemInformationSector; //Bytes 0x30 - 0x31
    uint16_t SectorNumberOfBackupBootSector; //Bytes 0x32 - 0x33
    //Bytes 0x34 - 0x3F Reserved
    uint16_t LogicalDriveNumberOfPartition; //Bytes 0x40 - 0x41 (0x41 usually unsused)
    uint8_t ExtendedSignature; //Bytes 0x42 //0x29
    uint32_t SerialNumberOfPartiton; //Bytes 0x43 - 0x46
    char VolumeNameOfPartition[11]; //Bytes 0x47 - 0x51
    char FATName[8]; //Bytes 0x52 - 0x59 (FAT32   )

    //Partition Tables
    MBR_PARTITION_ENTRY Entry1; //0x1BE
    MBR_PARTITION_ENTRY Entry2; //0x1CE
    MBR_PARTITION_ENTRY Entry3; //0x1DE
    MBR_PARTITION_ENTRY Entry4; //0x1EE

    //End
    uint16_t BootRecordSignature; //Bytes 0x1F3 - 0x1F4 //0x55AA
};

enum GUIDTypes{
    //Every
    UNSUSED_ENTRY = 0, //00000000-0000-0000-0000-000000000000
    MBR_PARTITION_SCHEME = 1, //024DEE41-33E7-11D3-9D69-0008C781F39F
    EFI_SYSTEM_PARTITION = 2, //C12A7328-F81F-11D2-BA4B-00A0C93EC93B
    BIOS_BOOT_PARITITON = 3, //NA //21686148-6449-6E6F-744E-656564454649
    INTEL_FAST_FLASH_PARTITION = 4, //NA //D3BFE2DE-3DAF-11DF-BA40-E3A556D89593

    //Microsoft
    MICROSOFT_RESERVED_PARITITON = 5, //NA //E3C9E316-0B5C-4DB8-817D-F92DF00215AE
    BASIC_DATA_PARITITON = 6, //FAT/NTFS //EBD0A0A2-B9E5-4433-87C0-68B6B72699C7
    STORAGE_SPACES_PARTITION = 7, //NA //E75CAF8F-F680-4CEE-AFA3-B001E56EFC2D

    //Linux
    LINUX_FILESYSTEM_DATA = 8, //EXT //0FC63DAF-8483-4772-8E79-3D69D8477DE4
    LINUX_ROOT = 9, //EXT //4F68BCE3-E8CD-4DB1-96E7-FBCAF984B709
    LINUX_BOOT = 10, //EXT //BC13C2FF-59E6-4262-A352-B275FD6F7172
    LINUX_SWAP = 11, //EXT //0657FD6D-A4AB-43C4-84E5-0933C84B4F4F
    LINUX_HOME = 12, //EXT //933AC7E1-2EB4-4F13-B844-0E14E2AEF915

    //MACOS
    APFS_CONTAINER = 13, //APFS //7C3457EF-0000-11AA-AA11-00306543ECAC
    APFS_RECOVERY = 14, //APFS //52637672-7900-11AA-AA11-00306543ECAC
};

struct GUID{
    uint32_t Entry1;
    uint16_t Entry2;
    uint16_t Entry3;
    uint16_t Entry4;
    uint16_t Entry5H;
    uint32_t Entry5L;

    GUIDTypes GetGUIDType();
    void SetGUID(GUIDTypes type);
};
bool operator ==(GUID l, GUID r);
bool operator ==(GUID l, GUIDTypes r);

struct GPT_PARTITION_ENTRY{
    GUID PartitionTypeGUID; //0x00 //0x00 is unused
    GUID PartitionGUID; //0x10
    uint64_t StartingLBA; //0x20
    uint64_t EndingLBA; //0x28
    uint64_t Attributes; //0x30
    char PartitionName[36]; //0x38 //Skip Odd (72 Bytes Long)

    bool Empty = true;
};

struct GPT{
    //Header
    char Signature[8]; //0x00 ("EFI PART")
    uint32_t Revision; //0x08
    uint32_t HeaderSize; //0x0C
    uint32_t CRC32Checksum; //0x10
    //0x14 4 Bytes Reserved
    uint64_t LBAOfHeader; //0x18
    uint64_t LBAOfAlternateHeader; //0x20
    uint64_t FirstUsableBlock; //0x28
    uint64_t LastUsableBlock; //0x30
    GUID GUIDMain; //0x38
    uint64_t StartingLBAofGUIDPartitionEntries; //0x48
    uint32_t NumberOfPartitionEntries; //0x50
    uint32_t SizeOfParitionEntries; //0x54 (Must Be a value of 128*2^n)
    uint32_t CRC32OfParitionEntry; //0x58
    //Reserverd (Rest Of Block)

    //Partiitons
    GPT_PARTITION_ENTRY* Entries;
    uint64_t EntryCount = 0;
};

struct FS_INFORMATION_SECTOR{
    uint32_t FirstSignature; //0x00 - 0x03
    //0x04 480 Null Bytes
    uint32_t SignatureOfFSInfoSector; //0x1E4 - 0x1E7 //(0x72 0x72 0x41 0x61)
    uint32_t NumberOfFreeClusters; //0x1E8 - 0x1EB //-1 if unknown
    uint32_t ClusterNumberOfMostRecentAllocation; //0x1EC - 0x1EF
    //0x1F0 12 Bytes Reserved
    uint16_t Unknown; //0x1FC - 0x1FD
    uint16_t BootRecordSignature; //0x1FE - 0x1FF //(0x55AA)
};

MBR_PARTITION_ENTRY ReadMBRPartition(void* Buffer, uint16_t Offset);
MBR* ReadMBR(void* Sector, MBR* Location);

GPT* ReadGPT(void* Sector, GPT* Location);
GPT* ReadGPTParitions(AHCI::Port* port, GPT* Location);