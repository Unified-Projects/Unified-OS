#ifndef __UNIFIED_OS_FS_DISKDEVICE_H
#define __UNIFIED_OS_FS_DISKDEVICE_H

#include <fs/PartitionDevice.h>

namespace UnifiedOS{
    namespace FileSystem{
        class DiskDevice{
            friend class PartitionDevice;

        protected:
            //Parition Counting
            int nextPartitionNumber = 0;

            char* DeviceName;

        private: //Direct Link with port (Will not work with Malloced buffers because of mapping)
            //8 x 4KiB buffers
            void* Buffer[8];

            //Reading blocks (512Bytes)
            virtual int ReadDiskBlock(uint64_t lba, size_t count, void* buffer);
            virtual int WriteDiskBlock(uint64_t lba, size_t count, void* buffer);

        public:
            DiskDevice();
            
            //Setup Partitions
            int InitializePartitions();

            //Reading bytes
            int Read(size_t off, size_t size, uint8_t* buffer);
            int Write(size_t off, size_t size, uint8_t* buffer);

            int ReadBlocks(uint64_t lba, size_t count, void* buffer);
            int WriteBlocks(uint64_t lba, size_t count, void* buffer);

            //Deconstructor
            ~DiskDevice();
            
            //Make into list
            PartitionDevice* partitions[16];
            int blocksize = 512;

            //Set Name (Port)
            void SetName(const char* name);
        private:
            int GetBuffer();
            int RelieveBuffer(uint8_t index);

            //Multiple Accesses at once
            uint8_t bufferLock;
        };
    }
}

#endif