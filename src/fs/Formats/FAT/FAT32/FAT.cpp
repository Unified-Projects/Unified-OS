#include <fs/Formats/FAT/FAT32/FAT.h>
#include <fs/Formats/FAT/FAT32/FATPartition.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;
using namespace UnifiedOS::FileSystem::FAT32;

#include <common/stdio.h>
#include <common/cstring.h>

FATClusterEntries* FAT32::ScanForEntries(Fat32Partition* Disk, uint64_t ClusterStartNumber){
    //Remove 2 (Cluster offsetting correction)
    if(ClusterStartNumber < 2){
        ClusterStartNumber = 2;
    }
    ClusterStartNumber -= 2;

    //Create a cluster vector
    FATClusterEntries* Clusters = new FATClusterEntries;

    //We work out if it's in the other FAT
    uint8_t ClusterAddition = 2;

    //Bool Fat2
    bool IsFat2 = false;

    //Check the sector and then add two to addtition if over the size of a FAT
    if(((ClusterStartNumber + ClusterAddition) / ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4))){
        ClusterAddition += 2;
        IsFat2 = true;
    }

    //Find First Sector Of Cluster
    //We add 2 as clusters start at entry 2
    uint64_t StartSector = ((ClusterStartNumber + ClusterAddition) / ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4)) + Disk->MBR.ReservedSectors + (IsFat2 * (Disk->MBR.SectorsPerFAT / Disk->MBR.SectorsPerCluster));

    //Read First Sector
    void* Sector = Memory::malloc(Disk->MBR.BytesPerSector);

    if(!(Disk->Read(Disk->MBR.BytesPerSector * StartSector, Disk->MBR.BytesPerSector, (uint8_t*)Sector))){
        return nullptr; //Disk read fail
    }

    //Current Entry
    uint64_t CurrentEntryPos = (ClusterStartNumber + ClusterAddition) % ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4);
    uint32_t CurrentEntry = (Endian::LittleEndian(Byte::Get4Byte(Sector, CurrentEntryPos * 4))) & 0xFFFFFF;

    //Add entry 1 witht
    Clusters->Entries.add_back((CurrentEntryPos - ClusterAddition) + 
                               (IsFat2 * ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4)) + 
                               ((Disk->MBR.ReservedSectors + (Disk->MBR.SectorsPerFAT * Disk->MBR.NumberOfFATs)) / Disk->MBR.SectorsPerCluster));
                               
    uint64_t PrevSector = StartSector;

    //Start Scanning Entries
    while(CurrentEntry != 0xFFFFFF){ //Stands for finished
        //Add the next entry
        Clusters->Entries.add_back((CurrentEntry - ClusterAddition) + 
                                    (IsFat2 * ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4)) + 
                                    ((Disk->MBR.ReservedSectors + (Disk->MBR.SectorsPerFAT * Disk->MBR.NumberOfFATs)) / Disk->MBR.SectorsPerCluster));

        //Check the new sector
        uint64_t NewSector = ((ClusterStartNumber + ClusterAddition) / ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4)) + Disk->MBR.ReservedSectors;

        if(NewSector != PrevSector){
            //Update comparison
            PrevSector = NewSector;

            //Read the new sector
            Disk->Read(StartSector * Disk->MBR.BytesPerSector, Disk->MBR.BytesPerSector, Sector);
        }

        //Updater 
        CurrentEntryPos = (CurrentEntry + ClusterAddition) % ((Disk->MBR.SectorsPerFAT * Disk->MBR.BytesPerSector) / 4);
        CurrentEntry = (Endian::LittleEndian(Byte::Get4Byte(Sector, CurrentEntryPos * 4))) & 0xFFFFFF;
    }

    //Free memory
    Memory::free(Sector);

    //Return
    return Clusters;
}