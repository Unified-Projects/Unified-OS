#include <fs/VolumeManager.h>
#include <memory/heap.h>
#include <memory/memory.h>
#include <common/cstring.h>
#include <common/stdio.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;
using namespace UnifiedOS::Memory;

Volume* VolumeManager::Volumes[255];
PartitionDevice* VolumeManager::Partitions[26];
uint8_t VolumeManager::NextFreeDiskMount = 0x00;
uint8_t VolumeManager::NextFreePartitionMount = 0x00;

//Volume
Volume::Volume(){

}

//Mounting
void Volume::Mount(uint8_t MountP, DiskDevice* disk){
    Mounted = true;
    MountPoint = MountP;

    Disk = disk;
}
//

//MANAGER

uint8_t VolumeManager::GetNextFreeDiskMount(){
    NextFreeDiskMount = (NextFreeDiskMount >= 26) ? NextFreeDiskMount - 26 : NextFreeDiskMount;
    if(IsDiskMountFree(NextFreeDiskMount)){
        NextFreeDiskMount += 1;
        return NextFreeDiskMount-1;
    }
    else{
        uint8_t count = 0;
        //Check all ports forward from Next free mount
        for(;count < 26; NextFreeDiskMount++, count++){
            //Stop out of bounding errors
            NextFreeDiskMount = (NextFreeDiskMount >= 26) ? NextFreeDiskMount - 26 : NextFreeDiskMount;

            if(IsDiskMountFree(NextFreeDiskMount)){
                return NextFreeDiskMount;
            }
        }
    }
    
    //Error
    return 0xFF;
}

//Returns whether a mount point is free
bool VolumeManager::IsDiskMountFree(uint8_t Mount){
    return (Volumes[Mount] == nullptr);
    // return (Volumes[Mount % 26] == nullptr) ? true : false;
}

uint8_t VolumeManager::GetNextFreePartitonMount(){
    NextFreePartitionMount = (NextFreePartitionMount >= 26) ? NextFreePartitionMount - 26 : NextFreePartitionMount;
    if(IsPartitionMountFree(NextFreePartitionMount)){
        NextFreePartitionMount += 1;
        return NextFreePartitionMount-1;
    }
    else{
        uint8_t count = 0;
        //Check all ports forward from Next free mount
        for(;count < 26; NextFreePartitionMount++, count++){
            //Stop out of bounding errors
            NextFreePartitionMount = (NextFreePartitionMount >= 26) ? NextFreePartitionMount - 26 : NextFreePartitionMount;

            if(IsPartitionMountFree(NextFreePartitionMount)){
                return NextFreePartitionMount;
            }
        }
    }
    
    //Error
    return 0xFF;
}
bool VolumeManager::IsPartitionMountFree(uint8_t Mount){
    return (Partitions[Mount] == nullptr);
}

VolumeManager::VolumeManager(){

}

//-1 Mount point means auto assign
bool VolumeManager::MountVolume(DiskDevice* disk){
    uint8_t M = GetNextFreeDiskMount();
    if(M != 0xFF){
        //Create a volume
        Volume* vol = new Volume();

        //Mount volume
        vol->Mount(M, disk);
        Volumes[M] = vol;

        //Initialise Volume
        vol->Disk->InitializePartitions();

        return true;
    }

    return false;
}

bool VolumeManager::MountPartition(PartitionDevice* partition, uint8_t MPoint = 0xFF){
    if(MPoint == 0xFF){
        uint8_t M = GetNextFreePartitonMount();

        //Check mount point is valid
        if(M != 0xFF && M < 26){
            //Mount Partition
            Partitions[M] = partition;

            //Run it's mount
            partition->MountPartition(M);

            return true;
        }
    }
    else{
        if(MPoint > 25){
            return false;
        }
        if(IsPartitionMountFree(MPoint)){
            //Mount Partition
            Partitions[MPoint] = partition;

            return true;
        }
    }

    return false;
}

//Reading from the disk
GeneralFile VolumeManager::ResolveFile(const char* Path){
    //Validate Mountpoint
    if(Path[0] - 0x41 < 0x1A){
        if(Partitions[Path[0] - 0x41]){ //Return the Resolved File
            return Partitions[Path[0] - 0x41]->ResolveFile(Path);
        }
    }

    return GeneralFile{};
}
GeneralDirectory VolumeManager::ResolveDir(const char* Path){
    //Validate Mountpoint
    if(Path[0] - 0x41 < 0x1A){
        if(Partitions[Path[0]]){ //Return the Resolve Directory
            return Partitions[Path[0]]->ResolveDir(Path);
        }
    }

    return GeneralDirectory{};
}

//Global Manager
VolumeManager* FileSystem::__FS_VOLUME_MANAGER__ = new VolumeManager();