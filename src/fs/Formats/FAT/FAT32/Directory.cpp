#include <fs/Formats/FAT/FAT32/Directory.h>
#include <fs/Formats/FAT/FAT32/FATPartition.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;
using namespace UnifiedOS::FileSystem::FAT32;

DirectoryEntryListing* FAT32::ReadDirectoryEntries(Fat32Partition* Disk, FATClusterEntries* Clusters){
    //Create a vector
    DirectoryEntryListing* ReturnValue = new DirectoryEntryListing;

    //Loop over Clusters
    for(uint64_t i = 0; i < Clusters->Entries.size(); i++){
        //Sectors
        uint64_t LBAs = Clusters->Entries.get_at(i) * Disk->MBR.SectorsPerCluster;

        //Data
        void* Data = Memory::malloc(Disk->MBR.BytesPerSector * Disk->MBR.SectorsPerCluster);

        //Read the data
        if(!(Disk->Read(Disk->MBR.BytesPerSector * LBAs, Disk->MBR.BytesPerSector * Disk->MBR.SectorsPerCluster, Data))){
            return nullptr; //Disk read fail
        }

        //Convert to DirEntries
        RootDirectoryEntry* RDEs = (RootDirectoryEntry*)Data;
        LongFileNameEntry* LFNEs = (LongFileNameEntry*)Data;

        //Create a beggining entry
        DirectoryEntry* Entry = new DirectoryEntry();

        for(size_t j = 0; j < (Disk->MBR.BytesPerSector * Disk->MBR.SectorsPerCluster) / 32; j++){
            if(RDEs[j].FileName[0] == 0x00 || RDEs[j].FileName[0] == 0xE5){ //Deleted or unallocated
                continue; //Skip
            }

            if(RDEs[j].FileAttributes & FAT32DirEntry_Long_File_NameM == 0x0F){ //LFNE
                Entry->LFNE.add_back(LFNEs[j]);
            }
            else{ //RDE
                Entry->RDE = RDEs[j];

                //Move on entry
                ReturnValue->Directories.add_back(Entry);

                //Reset Entry
                Entry = new DirectoryEntry;
            }
        }
    }

    //Return
    return ReturnValue;
}

bool FAT32::CheckForAttribute(uint8_t Attributes, FileAttributes AttributesMask, bool Specific){
    if(Specific){ //Only allow this attribute
        switch (AttributesMask)
        {
        case FAT32DirEntry_Read_OnlyM:
            return (Attributes == 1 << 0);
        case FAT32DirEntry_HiddenM:
            return (Attributes == 1 << 1);
        case FAT32DirEntry_System_FileM:
            return (Attributes == 1 << 2);
        case FAT32DirEntry_Volume_LabelM:
            return (Attributes == 1 << 3);
        case FAT32DirEntry_Long_File_NameM:
            return (Attributes == 0x0F);
        case FAT32DirEntry_DirectoryM:
            return (Attributes == 1 << 4);
        case FAT32DirEntry_ArchiveM:
            return (Attributes == 1 << 5);
        }
    }
    else{ //Allow other attributes
        switch (AttributesMask)
        {
        case FAT32DirEntry_Read_OnlyM:
            return ((Attributes & AttributesMask) >> 0 == 1);
        case FAT32DirEntry_HiddenM:
            return ((Attributes & AttributesMask) >> 1 == 1);
        case FAT32DirEntry_System_FileM:
            return ((Attributes & AttributesMask) >> 2 == 1);
        case FAT32DirEntry_Volume_LabelM:
            return ((Attributes & AttributesMask) >> 3 == 1);
        case FAT32DirEntry_Long_File_NameM:
            return ((Attributes & AttributesMask) >> 0 == 0x0F);
        case FAT32DirEntry_DirectoryM:
            return ((Attributes & AttributesMask) >> 4 == 1);
        case FAT32DirEntry_ArchiveM:
            return ((Attributes & AttributesMask) >> 5 == 1);
        }
    }
}