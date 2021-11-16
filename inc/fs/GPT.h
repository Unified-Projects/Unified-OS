#ifndef __UNIFIED_OS_FS_GPT_H
#define __UNIFIED_OS_FS_GPT_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace GPT{
            #define GPT_HEADER_SIGNATURE_LITTLE_ENDIAN 0x5452415020494645
            #define GPT_HEADER_SIGNATURE_BIG_ENDIAN 0x4546492050415254

            struct GPTHeader{
                uint64_t Signature;
                uint32_t Revision;
                uint32_t Size;
                uint32_t Crc32;
                uint32_t Reserved;
                uint64_t CurrentLBA;
                uint64_t BackupLBA;
                uint64_t FirstLBA;
                uint64_t LastLBA;
                uint8_t DiskGUID[16];
                uint64_t PartitionTableLBA;
                uint32_t PartNum;
                uint32_t PartEntrySize;
                uint32_t PartEntriesCRC;
            } __attribute__((packed));

            struct GPTPartitonEntry{
                uint8_t TypeGUID[16];
                uint8_t PartitionGUID[16];
                uint64_t StartLBA;
                uint64_t EndLBA;
                uint64_t Flags;
                uint8_t Name[72];
            } __attribute__((packed));

            int Parse(DiskDevice* disk);
        }
    }
}

#endif