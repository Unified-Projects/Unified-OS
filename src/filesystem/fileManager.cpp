#include "fileManager.h"
#include "ByteInteraction.h"
#include "../memory/heap.h"
#include "../Paging/PageFrameAllocator.h"

//Global FM
FileManager GlobalFileManager;

//Reads system entries
void FileManager::Initialize(){
    Drive* d = Drives[0];
    Partition* p = &d->Partitions[0];

    DIR::Directory TestDirectory(2, d, p);

    //Print Directory Files
    GlobalRenderer->PrintAtCursor("\n\n ---- Reading Root Directory ----\n\n");
    GlobalRenderer->Next();

    for(int i = 0; i < TestDirectory.SizeOfEntries; i++){
        if(TestDirectory.DirEntries[i].RDE.FileAttributes.Directory){
            GlobalRenderer->PrintAtCursor("Directory: ");
            GlobalRenderer->PrintAtCursor(TestDirectory.DirEntries[i].RDE.FileName);
            GlobalRenderer->Next();
        }
        else if(TestDirectory.DirEntries[i].RDE.FileAttributes.Volume_Label){
            GlobalRenderer->PrintAtCursor("Volume Name: ");
            GlobalRenderer->PrintAtCursor(TestDirectory.DirEntries[i].RDE.FileName);
            GlobalRenderer->Next();
        }
        else if(TestDirectory.DirEntries[i].RDE.FileAttributes.Archive){
            GlobalRenderer->PrintAtCursor("File: ");

            FILE::File f(TestDirectory.DirEntries[i], d, p);
            GlobalRenderer->PrintAtCursor(f.FileName);
            GlobalRenderer->PrintAtCursor(".");
            GlobalRenderer->PrintAtCursor(f.Extension);

            GlobalRenderer->Next();
        }
    }

    FILE::File test(TestDirectory.DirEntries[TestDirectory.SizeOfEntries - 1], d, p);

    //Print Test Title
    GlobalRenderer->PrintAtCursor("\n\n---- FILE TEST DATA READING ----\n\n");
    
    //Print File Name
    GlobalRenderer->PrintAtCursor("File Name: ");
    GlobalRenderer->PrintAtCursor(test.FileName);
    GlobalRenderer->PrintAtCursor(".");
    GlobalRenderer->PrintAtCursor(test.Extension);
    GlobalRenderer->Next();

    //Print File Size
    GlobalRenderer->PrintAtCursor("File Size: ");
    GlobalRenderer->PrintAtCursor(to_string((uint64_t)test.FileSize));
    GlobalRenderer->Next();

    //First Data Sector
    GlobalRenderer->PrintAtCursor("File Starting Sector: ");
    GlobalRenderer->PrintAtCursor(to_string((uint64_t)((test.DirEntry.RDE.Low2BytesOfAddressOfFirstCluster - 2) * p->PartitionMBR->SectorsPerCluster) + p->RDSector));
    GlobalRenderer->Next();
    GlobalRenderer->Next();

    //Test Reading Data
    test.ReadData();

    //Data
    GlobalRenderer->PrintAtCursor("File Data: \n");
    for(int i = 0; i < test.FileSize; i++){
        GlobalRenderer->putChar(((char*)test.data)[i]);
    }
    GlobalRenderer->Next();
}

//Validates and entry
bool ValidatePath(const char* path){
    if(sizeof(path)/sizeof(*path) >= 8){
        if(path[0] < NextMountable && path[0] >= 0x41){
            if(path[1] == ':'){
                if(path[2] == '\\' || path[2] == '/'){
                    return true;
                }
            }
        }
    }
    return false;
}

//Returns the file from a entry
FILE::File FileManager::FindFile(const char* path){
    //Create
    FILE::File file(DIRECTORY_ENTRY{}, NULL, NULL);

    //Validate
    if(!ValidatePath(path)) return file;

    Partition* partiton;
    Drive* drive;
    bool FoundPartition;

    //Get the partition
    for (int d = 0; d < DriveCount; d++){
        drive = Drives[d];

        for(int p = 0; p < drive->DriveGPT->EntryCount; p++){
            if(drive->Partitions[p].Mounted){
                if(drive->Partitions[p].MountPoint == path[0]){
                    FoundPartition = true;
                    partiton = &drive->Partitions[p];
                    break;
                }
            }
        }

        if(FoundPartition)
            break;
    }

    //Return
    return file;
}