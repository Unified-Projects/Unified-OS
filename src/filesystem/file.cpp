#include "file.h"
#include "ByteInteraction.h"
#include "../memory/heap.h"
#include "../Paging/PageFrameAllocator.h"

namespace FILE{
    //Reads the data from a directory entry
    File readFileData(DIRECTORY_ENTRY dirEntry, AHCI::Port* FileDrive){
        //Setup before reading
        File returnVal = File{};
        
          

        //Return
        return returnVal;
    }

    //Free used memory
    void File::FreeData(){
        //Check if empty already
        if(bufferPageCount == 0) return;

        //Free data and address
        GlobalAllocator.FreePages(data, bufferPageCount);

        //Clear counter
        bufferPageCount = 0;
    }   
    
    //
    //
    // READING DATA NEEDS TO BE WORKED ON
    // TRY TO INCLUDE OTHER CLUSTERS
    // RELY ON THE FAT TABLE OR
    // SOME FILES WONT READ ALL THE DATA !!!!
    //
    //

    void File::ReadData(){
        //If Full Stop or No Data/Empty File
        if(bufferPageCount > 0 && FileSize == 0) return;

        //Check Format
        switch (PartitionPtr->Format)
        {
        case FAT32:
            //Work out sizes
            bufferPageCount = FileSize / 4096 + 1;
            sectorCount = FileSize / PartitionPtr->PartitionMBR->BytesPerSector + 1;

            //Get memory space
            data = GlobalAllocator.RequestPages(bufferPageCount);
            memset(data, 0x41, FileSize);

            //FATCluster indexing
            uint64_t ClusterIndex = (FileSize / 
                                (PartitionPtr->PartitionMBR->BytesPerSector * PartitionPtr->PartitionMBR->SectorsPerCluster)) + 1;

            //Check if there is any page before reading into it
            if(data != NULL){
                //Read data
                for(int c = 0; c < ClusterIndex; c++){
                    DrivePtr->port->Read(((F32Entry.Clusters[c] - 2) * PartitionPtr->PartitionMBR->SectorsPerCluster) + PartitionPtr->RDSector, PartitionPtr->PartitionMBR->SectorsPerCluster, (void*)((uint8_t*)data + (c * (PartitionPtr->PartitionMBR->BytesPerSector * PartitionPtr->PartitionMBR->SectorsPerCluster))));
                }
            }
            break;
        }
    }

    //Null Entry
    File::File(){

    }

    File::File(DIRECTORY_ENTRY entry, Drive* drive, Partition* partition){
        if(!Setup && entry.Valid){
            //Naming Setup
            FileName = new char[255];
            Extension = new char[16];

            //Set Drive References
            DrivePtr = drive;
            PartitionPtr = partition;

            //Setup for different formats/*
            switch (partition->Format)
            {
            case FAT32:
                // If there are issues make sure to increase cluster number by 2 (remember it starts at 2)
                // OR Decrease
                // 
                // 
                // 
                
                //Intitalise the FAT entry
                F32Entry.Clusters = new uint64_t[(entry.RDE.FileSize/(partition->PartitionMBR->BytesPerSector * partition->PartitionMBR->SectorsPerCluster)) + 1];

                //Get the start entry number from Root Directory Entry
                uint32_t FATStartEntry = (((((uint32_t)entry.RDE.High2BytesOfAddressOfFirstCluster) << 16) & 0xFFFF0000) |
                                                       entry.RDE.Low2BytesOfAddressOfFirstCluster);

                //Calculate the sector
                uint64_t FatSectorIndex = FATStartEntry / (partition->PartitionMBR->BytesPerSector/4);
                
                //Calculate the index of the FAT entry within the sector
                uint64_t SectorIndex = FATStartEntry;
                while (SectorIndex > (partition->PartitionMBR->BytesPerSector/4) - 1){
                    SectorIndex -= (partition->PartitionMBR->BytesPerSector/4);
                }

                //Read the Fat Sector of start
                drive->port->Read(6302 + FatSectorIndex, 1, drive->port->buffer);

                //Get the FAT entry data
                uint32_t FATEntryData = Flip4Byte(Get4Byte(drive->port->buffer, SectorIndex * 4)) & 0x0FFFFFFF;

                //Current Entry
                uint32_t EntryIndex = 0;
                
                //Set First Entry
                F32Entry.Clusters[EntryIndex++] = FATStartEntry;

                //Get other entries
                while (FATEntryData != 0xFFFFFFF)
                {
                    //Set Next Entry
                    F32Entry.Clusters[EntryIndex++] = FATEntryData;

                    //Calculate the sector
                    FatSectorIndex = FATEntryData / (partition->PartitionMBR->BytesPerSector/4);

                    //Calculate the index of the FAT entry within the sector
                    SectorIndex = FATEntryData;
                    while (SectorIndex > (partition->PartitionMBR->BytesPerSector/4) - 1){
                        SectorIndex -= (partition->PartitionMBR->BytesPerSector/4);
                    }

                    //Read the Fat Sector of start
                    drive->port->Read(6302 + FatSectorIndex, 1, drive->port->buffer);

                    //Get the FAT entry data
                    FATEntryData = Flip4Byte(Get4Byte(drive->port->buffer, SectorIndex * 4)) & 0x0FFFFFFF;
                }
                
                //Rest of file info
                FileSize = entry.RDE.FileSize;

                //Set Entry
                DirEntry = entry;

                //LFNVersion
                if(entry.LFNEntries > 0){
                    //Values for locationing
                    bool extFound = false;
                    uint64_t extIndex = 0;
                    uint64_t fnIndex = 0;
                    uint64_t checkLength = 0;
                    char backupExt[16];

                    //Loop Over entries Backwards
                    for(int e = 15 - entry.LFNEntries + 1; e <= 15; e++){
                        //Get Entry
                        LFN_DRIECTORY_ENTRY l = (entry.LFNE[e]);
                        
                        //Loop Over Characters
                        for(int ci = 0; ci < 13; ci++){
                            if(l.FileName[ci] != 0x20){
                                //Check for "." as an extention starter
                                if(l.FileName[ci] == 0x2E){
                                    extFound = true;
                                    continue;
                                }

                                //Check if extention or not
                                if(extFound){
                                    //Ensure it is extention If not undo and put onto file name
                                    if(l.FileName[ci] == 0x2E){
                                        //Loop over characters
                                        while (checkLength > 0)
                                        {   
                                            //Modify values swapping
                                            FileName[fnIndex++] = l.FileName[ci];
                                            Extension[checkLength--] = 0x00;
                                        }
                                    }
                                    else{
                                        //Otherwise add to extention
                                        Extension[extIndex++] = l.FileName[ci];
                                        backupExt[checkLength++] = l.FileName[ci];
                                    }
                                }
                                else{
                                    //Add to filename
                                    FileName[fnIndex++] = l.FileName[ci];
                                }
                            }
                        }
                    }

                    //Set sizes
                    FileNameSize = fnIndex - 1;
                    ExtentionSize = extIndex - 1;
                }
                //NonLFN
                else{
                    //Get the filename(first 8) Ignore spaces
                    for(int ci = 0; ci < 8; ci++){
                        if(entry.RDE.FileName[ci] != 0x20){
                            FileName[FileNameSize++] = entry.RDE.FileName[ci];
                        }
                    }

                    //Get ext (Last 3)
                    for(int ci = 8; ci < 11; ci++){
                        if(entry.RDE.FileName[ci] != 0x20){
                            Extension[ExtentionSize++] = entry.RDE.FileName[ci];
                        }
                    }
                }

                break;
            }

            

            //Stop Repetition
            Setup = true;
        }
        
    }
}