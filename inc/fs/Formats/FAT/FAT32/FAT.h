#ifndef __UNIFIED_OS_FS_FORMAT_FAT_FAT32_FAT_H
#define __UNIFIED_OS_FS_FORMAT_FAT_FAT32_FAT_H

#include <common/stdint.h>
#include <common/vector.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace FAT32{
            class Fat32Partition; 

            struct FATClusterEntries
            {
                Vector<uint32_t> Entries;
            };

            FATClusterEntries* ScanForEntries(Fat32Partition* Disk, uint64_t ClusterStartNumber);
        }
    }
}

#endif