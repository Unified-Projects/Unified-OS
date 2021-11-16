#ifndef __UNIFIED_OS_FS_GPT_H
#define __UNIFIED_OS_FS_GPT_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace Endian{
            uint16_t BigEndian(uint16_t LittleEndian);
            uint32_t BigEndian(uint32_t LittleEndian);
            uint64_t BigEndian(uint64_t LittleEndian);

            uint16_t LittleEndian(uint16_t BigEndian);
            uint32_t LittleEndian(uint32_t BigEndian);
            uint64_t LittleEndian(uint64_t BigEndian);
        }

        namespace Byte{
            #define offsetBuffer(Buffer, bytes_to_skip) (Buffer += bytes_to_skip)

            //Definitions
            uint8_t GetByte(void* Buffer, uint64_t start);
            uint16_t Get2Byte(void* Buffer, uint64_t start);
            uint32_t Get4Byte(void* Buffer, uint64_t start);
            uint64_t Get8Byte(void* Buffer, uint64_t start);
        }
    }
}

#endif