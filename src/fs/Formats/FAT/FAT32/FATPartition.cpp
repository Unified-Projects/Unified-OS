#include <fs/Formats/FAT/FAT32/FATPartition.h>
#include <fs/Formats/FAT/FAT32/Directory.h>
#include <fs/Formats/FAT/FAT32/FAT.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;
using namespace UnifiedOS::FileSystem::FAT32;

#include <common/cstring.h> //Needed for path interaction

Fat32Partition::Fat32Partition(GPT::GPTPartitonEntry Partition, DiskDevice* disk, void* BootSector)
    : PartitionDevice(Partition.StartLBA, Partition.EndLBA, Partition.TypeGUID, Partition.PartitionGUID, disk),
      MBR(rMBR)
{
    //Save the boot sector
    rMBR = *((MBR::FAT32_MBR*)BootSector);

    //Read the flags for the drive
    rFlags.ReadOnly = (Partition.Flags >> 60 == 1);
    rFlags.Hidden = (Partition.Flags >> 62 == 1);
    rFlags.AutoMount = !(Partition.Flags >> 63 == 1);

    //Set the format
    rFormat = PARTITION_FAT_32;

    //Read Root Directory to get the volume name
    FATClusterEntries* RootClusters = ScanForEntries(this, 0);

    //Disk read check
    if(RootClusters){
        DirectoryEntryListing* RootDirectory = ReadDirectoryEntries(this, RootClusters);

        if(RootDirectory){
            //If LongFileName
            if(RootDirectory->Directories.get_at(0)->LFNE.size()){
                
            }
            else{//Normal file name
                //Setup name location
                PartitionName = new char[11];

                //Load data
                Memory::memcpy(PartitionName, &(RootDirectory->Directories.get_at(0)->RDE.FileName), 11);
            }

            delete RootDirectory;
        }

        delete RootClusters;
    }
}

GeneralFile Fat32Partition::ResolveFile(const char* Path){
    //Create the file
    GeneralFile File = {};

    //Check if the set Mount is correct
    if(Path[0] == Mount + 0x41){
        //Find out the length of the path
        size_t Length = strlen(Path);

        //Subdirectories
        uint64_t Subdirectories = 0;
        Vector<uint64_t> SectionSpacing = {};

        uint64_t CurrentSpacing = 0;

        //Calculate how many subdirectories are in the path
        for(int i = 0; i < Length; i++){
            if(Path[i] == '/' || Path[i] == '\\'){
                Subdirectories++;

                //Add spacing
                SectionSpacing.add_back(CurrentSpacing);
                CurrentSpacing = 0;
            }
            else{
                CurrentSpacing++;
            }
        }

        //Add spacing
        SectionSpacing.add_back(CurrentSpacing);
        CurrentSpacing = 0;

        //Remove first entry because its not a part of the filepath
        SectionSpacing.erase(0);

        //Remove RootDirectory as a subdir
        Subdirectories -= 1;

        //For memory comparison
        uint64_t OffsetIntoPath = 3; //Default to 3 for "A:/"

        //if it is a directory we want to store the new cluster
        uint32_t NewClusterPosition = 0;

        //First iterate over root directory
        //Read Root Directory to get the volume name
        FATClusterEntries* RootClusters = ScanForEntries(this, NewClusterPosition);

        //Disk read check
        if(RootClusters){
            DirectoryEntryListing* RootDirectory = ReadDirectoryEntries(this, RootClusters);

            if(RootDirectory){
                //Look over entries
                for(int i = 0; i < RootDirectory->Directories.size(); i++){
                    //To Save time compare if it should be a directory OR NOT
                    if((Subdirectories > 0 && CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false) && !CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_System_FileM, false)) ||
                        (Subdirectories == 0 && !CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false)  && !CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_System_FileM, false))){
                        
                        //Because the same process for creaing the file is here so I will make a bool for both to use
                        bool CorrectEntry = false;

                        //File name
                        char* EntryName = nullptr;

                        if(RootDirectory->Directories.get_at(i)->LFNE.size() > 0){//Compare as a long file name entry
                            //Creation of Long File Name From Entries

                            //Name Size
                            uint64_t NameSize = 0;

                            //Skipping character
                            bool Skip = false;

                            //Loop over LFNE entries and get the length of the s
                            for(int e = RootDirectory->Directories.get_at(i)->LFNE.size() - 1; e >= 0; e--){
                                for(int x = 0; x < 10; x++){
                                    if(!Skip && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0x00 && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0xFF){
                                        NameSize++;
                                    }
                                    Skip = !Skip;
                                }

                                for(int x = 0; x < 12; x++){
                                    if(!Skip && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0x00 && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0xFF){
                                        NameSize++;
                                    }
                                    Skip = !Skip;
                                }

                                for(int x = 0; x < 4; x++){
                                    if(!Skip && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0x00 && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0xFF){
                                        NameSize++;
                                    }
                                    Skip = !Skip;
                                }
                            }

                            //Reset Skip
                            Skip = false;

                            //Check lengths
                            if(NameSize == SectionSpacing.get_at(0)){ //Run further comparsions
                                EntryName = new char[NameSize];
                                uint8_t PosInName = 0;

                                //For File Extentions
                                bool CountedExtention = 0;

                                //Read the data into EntryName
                                for(int e = RootDirectory->Directories.get_at(i)->LFNE.size() - 1; e >= 0; e--){
                                    for(int x = 0; x < 10; x++){
                                        if(!Skip && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0x00 && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0xFF){
                                            EntryName[PosInName++] = RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x];
                                        }
                                        Skip = !Skip;
                                    }

                                    for(int x = 0; x < 12; x++){
                                        if(!Skip && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0x00 && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0xFF){
                                            EntryName[PosInName++] = RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x];
                                        }
                                        Skip = !Skip;
                                    }

                                    for(int x = 0; x < 4; x++){
                                        if(!Skip && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0x00 && RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0xFF){
                                            EntryName[PosInName++] = RootDirectory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x];
                                        }
                                        Skip = !Skip;
                                    }
                                }

                                //Comparison Time
                                if(Memory::memcmp(EntryName, ((uint8_t*)Path)+OffsetIntoPath, NameSize)){
                                    CorrectEntry = true;
                                }
                            }
                        }
                        else{//Root Directory Entry
                            //For size comparison
                            uint8_t EntrySize = 0;
                            
                            //Extention dot
                            if(!CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false)){
                                EntrySize += 1;
                            }

                            //Load the filename
                            for(int e = 0; e < 11; e++){
                                if(RootDirectory->Directories.get_at(i)->RDE.FileName[e] != 0x20){
                                    EntrySize++;
                                }
                            }

                            //Check lengths
                            if(EntrySize == SectionSpacing.get_at(0)){ //Run further comparsions
                                EntryName = new char[EntrySize];
                                uint8_t PosInName = 0;

                                //For File Extentions
                                bool CountedExtention = 0;

                                //Read the data
                                for(int e = 0; e < 11 && PosInName < EntrySize; e++){
                                    if(RootDirectory->Directories.get_at(i)->RDE.FileName[e] != 0x20){
                                        if(RootDirectory->Directories.get_at(i)->RDE.FileName[e] >= 0x41 && RootDirectory->Directories.get_at(i)->RDE.FileName[e] <= 0x5A)
                                            EntryName[PosInName++] = RootDirectory->Directories.get_at(i)->RDE.FileName[e]+ 0x20;
                                        else{
                                            EntryName[PosInName++] = RootDirectory->Directories.get_at(i)->RDE.FileName[e];
                                        }
                                    }
                                    else if(!CountedExtention && !CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false)){
                                        CountedExtention = true;

                                        //Add the decimal place
                                        EntryName[PosInName++] = '.';
                                    }
                                }

                                //Comparison Time
                                if(Memory::memcmp(EntryName, ((uint8_t*)Path)+OffsetIntoPath, EntrySize)){
                                    CorrectEntry = true;
                                }
                            }
                        }

                        //File creation
                        if(CorrectEntry){

                            //Cluster
                            NewClusterPosition = ((RootDirectory->Directories.get_at(i)->RDE.High2BytesOfAddressOfFirstCluster << 16) & 0xFF00) |
                                                    RootDirectory->Directories.get_at(i)->RDE.Low2BytesOfAddressOfFirstCluster;
                            
                            //Only entry needed (File is found)
                            if(Subdirectories == 0){
                                //Sort out the name
                                uint16_t ExtentionPos = 0;

                                //Find the position of the start of the Extention
                                for(int x = SectionSpacing.get_at(0) - 1; x >= 0; x--){
                                    if(EntryName[x] == '.'){
                                        ExtentionPos = x;
                                        break;
                                    }
                                }

                                if(ExtentionPos){
                                    //Copy the name data
                                    File.Name = new uint8_t[ExtentionPos];
                                    Memory::memcpy(File.Name, ((uint8_t*)EntryName), ExtentionPos);

                                    //Copy the extention
                                    File.Extention = new uint8_t[SectionSpacing.get_at(0) - ExtentionPos - 1];
                                    Memory::memcpy(File.Extention, ((uint8_t*)EntryName) + ExtentionPos + 1, SectionSpacing.get_at(0) - ExtentionPos - 1);
                                }
                                else{
                                    File.Name = new uint8_t[SectionSpacing.get_at(0)];
                                    Memory::memcpy(File.Name, ((uint8_t*)EntryName), SectionSpacing.get_at(0));
                                }

                                //Path Setup
                                File.FullPath = new uint8_t[strlen(Path)];
                                Memory::memcpy(File.FullPath, Path, strlen(Path));

                                //Basic file info
                                File.FileSize = RootDirectory->Directories.get_at(i)->RDE.FileSize;

                                //Entries
                                FATClusterEntries* FatEntries = ScanForEntries(this, NewClusterPosition);

                                //Setup the secotrs
                                File.Sectors = new uint64_t[FatEntries->Entries.size()*MBR.SectorsPerCluster];
                                File.SectorCount = FatEntries->Entries.size()*MBR.SectorsPerCluster; 

                                //Copy the sectors
                                for(int e = 0; e < FatEntries->Entries.size(); e++){
                                    for(int s = 0; s < MBR.SectorsPerCluster; s++){
                                        File.Sectors[(e * MBR.SectorsPerCluster) + s] = FatEntries->Entries.get_at(e) * MBR.SectorsPerCluster + s;
                                    }
                                }

                                //Disk Setup
                                File.Disk = this;

                                //Attributes
                                File.Attributes.Hidden = CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_HiddenM, false);
                                File.Attributes.Directory = CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false);
                                File.Attributes.ReadOnly = CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_Read_OnlyM, false);
                                File.Attributes.Archive = CheckForAttribute(RootDirectory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_ArchiveM, false);

                                //Validation
                                File.Found = true;

                                //End
                                delete FatEntries;
                                delete RootDirectory;
                                delete RootClusters;
                                return File;
                            }
                        }

                        //Memory Freeing
                        if(EntryName)
                            delete EntryName;
                    }
                }

                delete RootDirectory;
            }
            else{ //Fail Prevention
                delete RootClusters;
                return File;
            }

            delete RootClusters;
        }
        else{ //Fail Prevention
            return File;
        }

        //Now move onto any subdirectories
        for(int d = 0; d < Subdirectories; d++){
            //Offset the PathOffset
            OffsetIntoPath += SectionSpacing.get_at(d) + 1;

            //Read Directory to get the volume name
            FATClusterEntries* DirectoryClusters = ScanForEntries(this, NewClusterPosition);

            //Disk read check
            if(DirectoryClusters){
                DirectoryEntryListing* Directory = ReadDirectoryEntries(this, DirectoryClusters);

                if(Directory){
                    //Look over entries
                    for(int i = 0; i < Directory->Directories.size(); i++){
                        //To Save time compare if it should be a directory OR NOT
                        if((Subdirectories > d + 1 && CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false) && !CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_System_FileM, false)) ||
                            (Subdirectories == d + 1 && !CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false)  && !CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_System_FileM, false))){
                            
                            //Because the same process for creaing the file is here so I will make a bool for both to use
                            bool CorrectEntry = false;

                            //File name
                            char* EntryName = nullptr;

                            if(Directory->Directories.get_at(i)->LFNE.size() > 0){//Compare as a long file name entry
                                //Creation of Long File Name From Entries

                                //Name Size
                                uint64_t NameSize = 0;

                                //Skipping character
                                bool Skip = false;

                                //Loop over LFNE entries and get the length of the s
                                for(int e = Directory->Directories.get_at(i)->LFNE.size() - 1; e >= 0; e--){
                                    for(int x = 0; x < 10; x++){
                                        if(!Skip && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0x00 && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0xFF){
                                            NameSize++;
                                        }
                                        Skip = !Skip;
                                    }

                                    for(int x = 0; x < 12; x++){
                                        if(!Skip && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0x00 && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0xFF){
                                            NameSize++;
                                        }
                                        Skip = !Skip;
                                    }

                                    for(int x = 0; x < 4; x++){
                                        if(!Skip && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0x00 && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0xFF){
                                            NameSize++;
                                        }
                                        Skip = !Skip;
                                    }
                                }

                                //Reset Skip
                                Skip = false;

                                //Check lengths
                                if(NameSize == SectionSpacing.get_at(d+1)){ //Run further comparsions
                                    EntryName = new char[NameSize];
                                    uint8_t PosInName = 0;

                                    //For File Extentions
                                    bool CountedExtention = 0;

                                    //Read the data into EntryName
                                    for(int e = Directory->Directories.get_at(i)->LFNE.size() - 1; e >= 0; e--){
                                        for(int x = 0; x < 10; x++){
                                            if(!Skip && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0x00 && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x] != 0xFF){
                                                EntryName[PosInName++] = Directory->Directories.get_at(i)->LFNE.get_at(e).FileName1[x];
                                            }
                                            Skip = !Skip;
                                        }

                                        for(int x = 0; x < 12; x++){
                                            if(!Skip && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0x00 && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x] != 0xFF){
                                                EntryName[PosInName++] = Directory->Directories.get_at(i)->LFNE.get_at(e).FileName2[x];
                                            }
                                            Skip = !Skip;
                                        }

                                        for(int x = 0; x < 4; x++){
                                            if(!Skip && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0x00 && Directory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x] != 0xFF){
                                                EntryName[PosInName++] = Directory->Directories.get_at(i)->LFNE.get_at(e).FileName3[x];
                                            }
                                            Skip = !Skip;
                                        }
                                    }

                                    //Comparison Time
                                    if(Memory::memcmp(EntryName, ((uint8_t*)Path)+OffsetIntoPath, NameSize)){
                                        CorrectEntry = true;
                                    }
                                }
                            }
                            else{//Root Directory Entry
                                //For size comparison
                                uint8_t EntrySize = 0;
                                
                                //Extention dot
                                if(!CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false)){
                                    EntrySize += 1;
                                }

                                //Load the filename
                                for(int e = 0; e < 11; e++){
                                    if(Directory->Directories.get_at(i)->RDE.FileName[e] != 0x20){
                                        EntrySize++;
                                    }
                                }

                                //Check lengths
                                if(EntrySize == SectionSpacing.get_at(d + 1)){ //Run further comparsions
                                    EntryName = new char[EntrySize];
                                    uint8_t PosInName = 0;

                                    //For File Extentions
                                    bool CountedExtention = 0;

                                    //Read the data
                                    for(int e = 0; e < 11 && PosInName < EntrySize; e++){
                                        if(Directory->Directories.get_at(i)->RDE.FileName[e] != 0x20){
                                            if(Directory->Directories.get_at(i)->RDE.FileName[e] >= 0x41 && Directory->Directories.get_at(i)->RDE.FileName[e] <= 0x5A)
                                                EntryName[PosInName++] = Directory->Directories.get_at(i)->RDE.FileName[e]+ 0x20;
                                            else{
                                                EntryName[PosInName++] = Directory->Directories.get_at(i)->RDE.FileName[e];
                                            }
                                        }
                                        else if(!CountedExtention && !CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false)){
                                            CountedExtention = true;

                                            //Add the decimal place
                                            EntryName[PosInName++] = '.';
                                        }
                                    }

                                    //Comparison Time
                                    if(Memory::memcmp(EntryName, ((uint8_t*)Path)+OffsetIntoPath, EntrySize)){
                                        CorrectEntry = true;
                                    }
                                }
                            }

                            //File creation
                            if(CorrectEntry){

                                //Cluster
                                NewClusterPosition = ((Directory->Directories.get_at(i)->RDE.High2BytesOfAddressOfFirstCluster << 16) & 0xFF00) |
                                                        Directory->Directories.get_at(i)->RDE.Low2BytesOfAddressOfFirstCluster;
                                
                                //Only entry needed (File is found)
                                if(Subdirectories == d+1){
                                    //Sort out the name
                                    uint16_t ExtentionPos = 0;

                                    //Find the position of the start of the Extention
                                    for(int x = SectionSpacing.get_at(d+1) - 1; x >= 0; x--){
                                        if(EntryName[x] == '.'){
                                            ExtentionPos = x;
                                            break;
                                        }
                                    }

                                    if(ExtentionPos){
                                        //Copy the name data
                                        File.Name = new uint8_t[ExtentionPos];
                                        Memory::memcpy(File.Name, ((uint8_t*)EntryName), ExtentionPos);

                                        //Copy the extention
                                        File.Extention = new uint8_t[SectionSpacing.get_at(d+1) - ExtentionPos - 1];
                                        Memory::memcpy(File.Extention, ((uint8_t*)EntryName) + ExtentionPos + 1, SectionSpacing.get_at(d+1) - ExtentionPos - 1);
                                    }
                                    else{
                                        File.Name = new uint8_t[SectionSpacing.get_at(d+1)];
                                        Memory::memcpy(File.Name, ((uint8_t*)EntryName), SectionSpacing.get_at(d+1));
                                    }

                                    //Path Setup
                                    File.FullPath = new uint8_t[strlen(Path)];
                                    Memory::memcpy(File.FullPath, Path, strlen(Path));

                                    //Basic file info
                                    File.FileSize = Directory->Directories.get_at(i)->RDE.FileSize;

                                    //Entries
                                    FATClusterEntries* FatEntries = ScanForEntries(this, NewClusterPosition);

                                    //Setup the secotrs
                                    File.Sectors = new uint64_t[FatEntries->Entries.size()*MBR.SectorsPerCluster];
                                    File.SectorCount = FatEntries->Entries.size()*MBR.SectorsPerCluster; 

                                    //Copy the sectors
                                    for(int e = 0; e < FatEntries->Entries.size(); e++){
                                        for(int s = 0; s < MBR.SectorsPerCluster; s++){
                                            File.Sectors[(e * MBR.SectorsPerCluster) + s] = FatEntries->Entries.get_at(e) * MBR.SectorsPerCluster + s;
                                        }
                                    }

                                    //Disk Setup
                                    File.Disk = this;

                                    //Attributes
                                    File.Attributes.Hidden = CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_HiddenM, false);
                                    File.Attributes.Directory = CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_DirectoryM, false);
                                    File.Attributes.ReadOnly = CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_Read_OnlyM, false);
                                    File.Attributes.Archive = CheckForAttribute(Directory->Directories.get_at(i)->RDE.FileAttributes, FAT32DirEntry_ArchiveM, false);

                                    //Validation
                                    File.Found = true;

                                    //End
                                    // delete FatEntries;
                                    delete Directory;
                                    delete DirectoryClusters;
                                    return File;
                                }
                            }

                            //Memory Freeing
                            if(EntryName)
                                delete EntryName;
                        }
                    }

                    delete Directory;
                }
                else{ //Fail Prevention
                    delete DirectoryClusters;
                    return File;
                }

                delete DirectoryClusters;
            }
            else{ //Fail Prevention
                return File;
            }
        }
    }

    //return
    return File;
}
GeneralDirectory Fat32Partition::ResolveDir(const char* Path){
    //Create the directory
    GeneralDirectory Directory = {};

    

    //return
    return Directory;
}