#ifndef __UNIFIED_OS_FS_VOLUMEMANAGER_H
#define __UNIFIED_OS_FS_VOLUMEMANAGER_H

#include <common/stdint.h>

#include <fs/DiskDevice.h>

namespace UnifiedOS{
    namespace FileSystem{
        class Volume{
            friend DiskDevice;
        protected:
            //Mounting
            bool Mounted = false;
            uint8_t MountPoint = 0xFF;

        public:
            //Disk Device
            DiskDevice* Disk;
        public:
            Volume();

            //Mounting
            void Mount(uint8_t MountP, DiskDevice* disk);
            void Mount(uint8_t MountP, PartitionDevice* disk);
        };

        class VolumeManager{
        protected:
            //Volumes
            static Volume* Volumes[255]; //Disks

            //Mounted Partitions
            static PartitionDevice* Partitions[26];
            
            //Speeds up mounting with next free (May be filled with a custom Mount Point so we check first)
            static uint8_t NextFreeDiskMount;
            static uint8_t NextFreePartitionMount;

        private:
            uint8_t GetNextFreeDiskMount();
            bool IsDiskMountFree(uint8_t Mount);

            uint8_t GetNextFreePartitonMount();
            bool IsPartitionMountFree(uint8_t Mount);

        public:
            VolumeManager();

            //-1 Mount point means auto assign
            bool MountVolume(DiskDevice* volume);
            bool MountPartition(PartitionDevice* partition, uint8_t MPoint = 0xFF);
        };

        //Global
        extern VolumeManager* __FS_VOLUME_MANAGER__;
    }
}

#endif