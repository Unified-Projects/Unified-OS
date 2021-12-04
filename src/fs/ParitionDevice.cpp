#include <fs/PartitionDevice.h>
#include <fs/DiskDevice.h>

#include <memory/memory.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;

//Check the type of the disk to work out what type of format

//Defaults
PartitionDevice::PartitionDevice(uint64_t startLBA, uint64_t endLBA, uint8_t typeGUID[16], uint8_t partitionGUID[16], DiskDevice* disk)
    : FreeSpace(rFreeSpace), PartitionSize(rPartitionSize), Format(rFormat),
      Mount(rMount), Mounted(rMounted), Flags(rFlags)
{
    //Partitions Position
    StartLBA = startLBA;
    EndLBA = endLBA;

    //For reading
    ParentDisk = disk;

    //GUID
    Memory::memcpy(&TypeGUID, &typeGUID, 16);
    Memory::memcpy(&PartitionGUID, &partitionGUID, 16);
}

int PartitionDevice::ReadAbsolute(uint64_t off, size_t count, void* buffer){
    ParentDisk->Read(off + (0x200 * StartLBA), count * 0x200, (uint8_t*)buffer);
}
int PartitionDevice::ReadBlock(uint64_t lba, size_t count, void* buffer){
    ParentDisk->ReadBlocks(lba + (StartLBA), count, buffer);
}
int PartitionDevice::WriteBlock(uint64_t lba, size_t count, void* buffer){
    ParentDisk->WriteBlocks(lba + (StartLBA), count, buffer);
}

int PartitionDevice::Read(size_t off, size_t size, uint8_t* buffer){
    ParentDisk->Read(off + (0x200 * StartLBA), size, buffer);
}
int PartitionDevice::Write(size_t off, size_t size, uint8_t* buffer){
    ParentDisk->Write(off + (0x200 * StartLBA), size, buffer);
}

//Resolve Structures
GeneralFile PartitionDevice::ResolveFile(const char* Path){
    //Create the file
    GeneralFile File = {};

    //return
    return File;
}
GeneralDirectory PartitionDevice::ResolveDir(const char* Path){
    //Create the directory
    GeneralDirectory Directory = {};

    //return
    return Directory;
}

PartitionDevice::~PartitionDevice(){}

void PartitionDevice::MountPartition(uint8_t MountPoint){
    this->rMount = MountPoint;
    this->rMounted = true;
}