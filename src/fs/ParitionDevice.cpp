#include <fs/PartitionDevice.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;

//Defaults
PartitionDevice::PartitionDevice(uint64_t startLBA, uint64_t endLBA, DiskDevice* disk)
    : FreeSpace(rFreeSpace), PartitionSize(rPartitionSize), Format(rFormat),
      Mount(rMount), Mounted(rMounted)
{
    //Partitions Position
    StartLBA = startLBA;
    EndLBA = endLBA;

    //For reading
    parentDisk = disk;
}

int PartitionDevice::ReadAbsolute(uint64_t off, size_t count, void* buffer){}
int PartitionDevice::ReadBlock(uint64_t lba, size_t count, void* buffer){}
int PartitionDevice::WriteBlock(uint64_t lba, size_t count, void* buffer){}

int PartitionDevice::Read(size_t off, size_t size, uint8_t* buffer){}
int PartitionDevice::Write(size_t off, size_t size, uint8_t* buffer){}

PartitionDevice::~PartitionDevice(){}

void PartitionDevice::MountPartition(uint8_t MountPoint){
    this->rMount = MountPoint;
    this->rMounted = true;
}