#include "directory.h"
#include "ByteInteraction.h"
#include "../Paging/PageFrameAllocator.h"

using namespace DIR;

Directory::Directory(){

}

Directory::Directory(uint64_t FirstCluster, Drive* drive, Partition* partition){
    if(!Setup){
        //Main Refernces
        DrivePtr = drive;
        PartitionPtr = partition;

        //Check Partition Type
        switch (partition->Format)
        {
        case FAT32:
            //
            //First Read FAT
            //

            //
            //
            //
            //
            //  HARD LIMITED CLUSTER SIZE(SHOULD CHANGE TO DYNAMIC)
            //
            //
            //
            //

            //Intitalise the FAT entry
            F32Entry.Clusters = new uint64_t[32];

            //Get the start entry number from Root Directory Entry
            uint32_t FATStartEntry = FirstCluster;

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
            
            //Set First Entry
            F32Entry.Clusters[Entries++] = FATStartEntry;

            //Get other entries
            while (FATEntryData != 0xFFFFFFF)
            {
                //Set Next Entry
                F32Entry.Clusters[Entries++] = FATEntryData;

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

            //
            //  Create Entries
            //
            
            //Sizing
            DirEntries = new DIRECTORY_ENTRY[((Entries * partition->PartitionMBR->SectorsPerCluster) * partition->PartitionMBR->BytesPerSector) / 32];

            //Generate Buffer
            void* Buffer = GlobalAllocator.RequestPages(((Entries * partition->PartitionMBR->SectorsPerCluster) * partition->PartitionMBR->BytesPerSector) / 4096 + 1);

            if(Buffer != NULL){
                //Read Data into buffer
                for(int c = 0; c < Entries; c++){
                    DrivePtr->port->Read(((F32Entry.Clusters[c] - 2) * PartitionPtr->PartitionMBR->SectorsPerCluster) + PartitionPtr->RDSector, PartitionPtr->PartitionMBR->SectorsPerCluster, (void*)((uint8_t*)Buffer + (c * (PartitionPtr->PartitionMBR->BytesPerSector * PartitionPtr->PartitionMBR->SectorsPerCluster))));
                }

                //Offsetting
                uint64_t offset = 0;

                //Read Entries
                for(int e = 0; e < ((Entries * partition->PartitionMBR->SectorsPerCluster) * partition->PartitionMBR->BytesPerSector) / 32; e++){
                    //Entry
                    DIRECTORY_ENTRY ent = RootGetDirectoryEntry(Buffer, offset);

                    //Validate
                    if(ent.Valid){
                        //Apply Entry
                        DirEntries[SizeOfEntries] = ent;

                        //Skip over LFN entries
                        offset += ent.LFNEntries * 32;

                        //For Reference
                        SizeOfEntries++;
                    }

                    //Move to next entry
                    offset += 32;
                }
            }

            //Free Buffer
            GlobalAllocator.FreePages(Buffer, ((Entries * partition->PartitionMBR->SectorsPerCluster) * partition->PartitionMBR->BytesPerSector) / 4096 + 1);

            break;
        }
    }
}