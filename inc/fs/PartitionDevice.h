#ifndef __UNIFIED_OS_FS_PARTITONDEVICE_H
#define __UNIFIED_OS_FS_PARTITONDEVICE_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace FileSystem{
        class DiskDevice;

        enum PartitionFormats{
            PARTITION_UNKOWN,
            PARTITION_FAT_32
        };

        class PartitionDevice{
        protected:
            char* PartitionName;
        public:
            //Setup of a partition
            PartitionDevice(uint64_t startLBA, uint64_t endLBA, DiskDevice* disk);

            //Read/Writing of a block at an offset
            virtual int ReadAbsolute(uint64_t off, size_t count, void* buffer);

            //Read Blocks of the partitions (Based on start/end)
            virtual int ReadBlock(uint64_t lba, size_t count, void* buffer);
            virtual int WriteBlock(uint64_t lba, size_t count, void* buffer);

            //Read/Write to a specific position
            virtual int Read(size_t off, size_t size, uint8_t* buffer);
            virtual int Write(size_t off, size_t size, uint8_t* buffer);

            //Deconstructor
            ~PartitionDevice();

            //Disk
            DiskDevice* parentDisk;

        public: //Mounting
            void MountPartition(uint8_t MountPoint);
            const uint8_t& Mount;
            const bool& Mounted;

        public: //Public Info (Read Only)
            const uint64_t& PartitionSize;
            const uint64_t& FreeSpace;

            const PartitionFormats& Format;

        protected: //Partition Info
            uint64_t rPartitionSize;
            uint64_t rFreeSpace;

            PartitionFormats rFormat = PARTITION_UNKOWN;

            uint8_t rMount;
            bool rMounted;

        private:
            //Reference to the positions of the parition
            uint64_t StartLBA;
            uint64_t EndLBA;
        };
    }
}

#endif