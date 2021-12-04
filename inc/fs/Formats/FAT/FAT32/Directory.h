#ifndef __UNIFIED_OS_FS_FORMAT_FAT_FAT32_DIRECTORY_H
#define __UNIFIED_OS_FS_FORMAT_FAT_FAT32_DIRECTORY_H

#include <common/stdint.h>

#include <fs/Formats/FAT/FAT32/FAT.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace FAT32{
            class Fat32Partition; 

            enum FileAttributes{
                FAT32DirEntry_Read_OnlyM = 0x01, //Mask with this and bit shift >> 0 and if == 1 its toggled
                FAT32DirEntry_HiddenM = 0x02, //Mask with this and bit shift >> 1 and if == 1 its toggled
                FAT32DirEntry_System_FileM = 0x04, //Mask with this and bit shift >> 2 and if == 1 its toggled
                FAT32DirEntry_Volume_LabelM = 0x08, //Mask with this and bit shift >> 3 and if == 1 its toggled
                FAT32DirEntry_Long_File_NameM = 0x0F, //Mask with this and bit shift >> 0 and if == 15 its toggled
                FAT32DirEntry_DirectoryM = 0x10, //Mask with this and bit shift >> 4 and if == 1 its toggled
                FAT32DirEntry_ArchiveM = 0x20 //Mask with this and bit shift >> 5 and if == 1 its toggled
            };

            struct LongFileNameEntry{
                uint8_t SequenceNumber; //Status's (Starts at 0x01 and ends at 0x40) (if not these then its empty)
                uint8_t FileName1[10]; //Note for these names
                                       //There is 1 character then one filler character
                                       //So like "F i l e N a m e . t x t"
                                       //This is not right im missing some gasp like file attributes
                uint8_t FileAttributes;
                uint8_t Unused;
                uint8_t Checksum;
                uint8_t FileName2[12];
                uint16_t Reserved;
                uint8_t FileName3[4];
            } __attribute__((packed));

            struct RootDirectoryEntry
            {
                //Byte zero if FileName is the Status (0x00 = unallocated) (0xe5 = deleted)
                uint8_t FileName[11]; //If Not using allocation status (We will add First Char)
                uint8_t FileAttributes;
                uint8_t Reserved;
                uint8_t FileCreationTime; //Tenth of seconds //Not Used By DOS
                uint16_t CreationTime; //(Hours, Minutes, Seconds) //Not Used By DOS
                uint16_t CreationDate; //Not Used By DOS
                uint16_t AccessDate; //Not Used By DOS
                uint16_t High2BytesOfAddressOfFirstCluster; //0 for FAT12/FAT16
                uint16_t ModifiedTime; //(Hours, Minutes, Seconds)
                uint16_t ModifiedDate;
                uint16_t Low2BytesOfAddressOfFirstCluster;
                uint32_t FileSize; //0 For Directories
            } __attribute__((packed));

            //
            //
            //
            //
            //
            //
            //Potential Memory Issue here and with all vectors
            //Have a look into memory being deleted because I dont think it does.
            //I also rely on there being no corruption so checksum algorthim should be used
            //
            //
            //
            //
            //
            //

            struct DirectoryEntry{
                RootDirectoryEntry RDE;

                Vector<LongFileNameEntry> LFNE;  /*Last == First (They Stack, Its just how it is in the actual
                                                                 data of the drive the last is first so I need to flip)*/
            };

            struct DirectoryEntryListing
            {
                Vector<DirectoryEntry*> Directories;
            };

            DirectoryEntryListing* ReadDirectoryEntries(Fat32Partition* Disk, FATClusterEntries* Clusters);

            bool CheckForAttribute(uint8_t Attributes, FileAttributes AttributesMask, bool Specific);
        }
    }
}

#endif