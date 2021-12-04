#ifndef __UNIFIED_OS_FS_FORMAT_FAT_FAT32_PARTITION_H
#define __UNIFIED_OS_FS_FORMAT_FAT_FAT32_PARTITION_H

#include <common/stdint.h>
#include <fs/PartitionDevice.h>
#include <fs/MBR.h>
#include <fs/GPT.h>

#include <fs/Formats/FAT/FAT32/FAT.h>
#include <fs/Formats/FAT/FAT32/Directory.h>
#include <fs/Formats/FAT/FAT32/File.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace FAT32{
            class Fat32Partition : public PartitionDevice{
            public:
                Fat32Partition(GPT::GPTPartitonEntry Partition, DiskDevice* disk, void* BootSector);

                //Resolve Structures
                GeneralFile ResolveFile(const char* Path); //Returns a file object containing info
                GeneralDirectory ResolveDir(const char* Path); //Returns a list of directory entries
            public:
                const MBR::FAT32_MBR& MBR;
            private:
                MBR::FAT32_MBR rMBR;
            };
        }
    }
}

#endif