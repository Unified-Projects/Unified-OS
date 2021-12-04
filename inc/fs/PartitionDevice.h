#ifndef __UNIFIED_OS_FS_PARTITONDEVICE_H
#define __UNIFIED_OS_FS_PARTITONDEVICE_H

#include <common/stdint.h>

#include <fs/GeneralStructs.h>

namespace UnifiedOS{
    namespace FileSystem{
        class DiskDevice;

        enum PartitionFormats{
            PARTITION_UNKOWN,
            PARTITION_FAT_32
        };

        class PartitionDevice{
            struct PartitionFlags
            {
                bool Hidden;
                bool AutoMount;
                bool ReadOnly;
            };
            
        public:
            char* PartitionName;
        public:
            //Setup of a partition
            PartitionDevice(uint64_t startLBA, uint64_t endLBA, uint8_t TypeGUID[16], uint8_t PartitionGUID[16], DiskDevice* disk);

            //Read/Writing of a block at an offset
            virtual int ReadAbsolute(uint64_t off, size_t count, void* buffer);

            //Read Blocks of the partitions (Based on start/end)
            virtual int ReadBlock(uint64_t lba, size_t count, void* buffer);
            virtual int WriteBlock(uint64_t lba, size_t count, void* buffer);

            //Read/Write to a specific position
            virtual int Read(size_t off, size_t size, uint8_t* buffer);
            virtual int Write(size_t off, size_t size, uint8_t* buffer);

            //Resolve Structures
            virtual GeneralFile ResolveFile(const char* Path); //Returns a file object containing info
            virtual GeneralDirectory ResolveDir(const char* Path); //Returns a list of directory entries

            //Need to introduce writing to the disk and modifying data. This will require me to reorganise the file data
            // virtual int WriteFile(GeneralFile File, uint8_t* NewData); //Will remove / add new clusters to the data

            //Deconstructor
            ~PartitionDevice();

            //Disk
            DiskDevice* ParentDisk;

        public: //Mounting
            void MountPartition(uint8_t MountPoint);
            const uint8_t& Mount;
            const bool& Mounted;

            const PartitionFlags& Flags;

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

            PartitionFlags rFlags;

            uint8_t TypeGUID[16];
            uint8_t PartitionGUID[16];

        private:
            //Reference to the positions of the parition
            uint64_t StartLBA;
            uint64_t EndLBA;
        };
    }
}

#endif