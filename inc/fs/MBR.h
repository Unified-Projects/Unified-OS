#ifndef __UNIFIED_OS_FS_MBR_H
#define __UNIFIED_OS_FS_MBR_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace MBR{
            //CURRENTLY UNSUPPORTED!!! GPT Only

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
            } __attribute__((packed));

            //Other MBR Stuff after Parameter block
                //BootstrapArea
                // uint8_t BootstrapArea[127];

                // //Disk Timestamp
                // uint16_t TimeStamp_Empty; //0x0000
                // uint8_t OriginalDrive; //0x80 - 0xFF
                // uint8_t Seconds; //0-59
                // uint8_t Minutes; //0-59
                // uint8_t Hours; //0-23

                // //Bootstrap Area Part 2
                // uint8_t BootstrapArea2[216];

                // //Signature
                // uint32_t Signature32;
                // uint16_t CopyProtected; //0x0000 Not, 0x5A5A yes

                // //Paritions
                // MBR_PARTITION_ENTRY Partition1;
                // MBR_PARTITION_ENTRY Partition2;
                // MBR_PARTITION_ENTRY Partition3;
                // MBR_PARTITION_ENTRY Partition4;

            struct FAT32_MBR{
                uint8_t JMPInstruction[3];
                uint8_t OEM[8];

                //BIOS ParameterBlock
                uint16_t BytesPerSector;
                uint8_t SectorsPerCluster;
                uint16_t ReservedSectors;
                uint8_t NumberOfFATs;
                uint8_t Unused1[2];
                uint8_t Unused2[2];
                uint8_t MediaDescriptor;
                uint8_t Unused3[2];
                uint16_t SectorsPerTrack;
                uint16_t NumberOfHeads;
                uint32_t HiddenSectors;
                uint32_t TotalSectors;
                uint32_t SectorsPerFAT;
                uint16_t ExtendedFlags;
                uint16_t Version;
                uint32_t RootCluster;
                uint16_t SystemInformationSector;
                uint16_t BackupBootSector;
                uint8_t Unused4[12];

                //Extended BIOS ParamterBlock
                uint8_t PhysicalDrive;
                uint8_t Reserved;
                uint8_t ExtendedSignature;
                uint32_t SerialNumber;
                uint8_t VolumeLabel[11];
                uint64_t FileSystem; // 0x4641543332202020

                //Offset of 90

                //Useless for now
                uint8_t BootstrapCode[420];

                //Signature
                uint16_t Signature; //0x55AA

            } __attribute__((packed));
                        
        }
    }
}

#endif