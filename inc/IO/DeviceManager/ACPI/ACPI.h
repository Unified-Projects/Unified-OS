#ifndef __UNIFIED_OS_IO_DEVICE__H
#define __UNIFIED_OS_IO_DEVICE__H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Devices{
        namespace ACPI{
            struct SDTHeader{
                unsigned char Signature[4];
                uint32_t Length;
                uint8_t Revision;
                uint8_t Checksum;
                uint8_t OEMId[6];
                uint8_t OEMTableId[8];
                uint32_t OEMRevision;
                uint32_t CreatorId;
                uint32_t CreatorRevision;
            } __attribute__((packed));

            struct MCFGHeader{
                SDTHeader Header;
                uint64_t Reserved;
            } __attribute__((packed));

            struct DeviceConfig{
                uint64_t BaseAddress;
                uint16_t PCISegGroup;
                uint8_t StartBus;
                uint8_t EndBus;
                uint32_t Reserved;
            } __attribute__((packed));

            void* FindTable(SDTHeader* sdtHeader, char* signature);
        }
    }
}

#endif