#ifndef __UNIFIED_OS_FS_GENERALSTRUCTS_H
#define __UNIFIED_OS_FS_GENERALSTRUCTS_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace FileSystem{
        class PartitionDevice;

        struct GeneralAttributes
        {
            bool Hidden;
            bool Directory;
            bool ReadOnly;
            bool Archive;
        };

        struct GeneralFile
        {
            //Info
            uint8_t* Name;
            uint8_t* Extention; //Directories dont use this

            uint8_t* FullPath;

            uint64_t FileSize;

            //File Data Location
            uint64_t* Sectors;
            uint64_t SectorCount;

            //Attributes
            GeneralAttributes Attributes;

            //For Read/Write
            PartitionDevice* Disk;

            //Empty
            bool Found = false;

            ~GeneralFile(){
                delete Name;
                delete Extention;

                delete FullPath;

                delete Sectors;
            }
        };

        struct GeneralDirectory
        {
            //Info
            uint8_t* Name;

            uint8_t* FullPath;

            //Entries
            GeneralFile* Entries;
            uint64_t EntryCount;

            //For Read/Write
            PartitionDevice* Disk;

            //Empty
            bool Found = false;
            
            ~GeneralDirectory(){
                delete Name;
                
                delete FullPath;

                delete Entries;
            }
        };
    }
}

#endif