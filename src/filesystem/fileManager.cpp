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

    d->port->Read(8321, 1, d->port->buffer);

    ROOT_DIRECTORY_ENTRY rde = GetRDE(d->port->buffer, 0x00);
    uint32_t FATStartEntry = ((((uint32_t)rde.High2BytesOfAddressOfFirstCluster) << 16) & 0xFFFF0000) | rde.Low2BytesOfAddressOfFirstCluster;
    FAT32Entry End;

    uint32_t entry = Flip4Byte(FATStartEntry);
    //entry = 0x01000000;

    uint64_t index = (Flip4Byte(entry) * 32) / 512 + 1;

    d->port->Read(6301 + index, 1, d->port->buffer);

    uint32_t pos = Flip4Byte(entry);

    while (pos >= 16){
        pos -= 16;
    }

    entry = Get4Byte(d->port->buffer, pos * 32);

    uint64_t EntrySize = 0x01;

    while((Flip4Byte(entry) & 0xFFFFFFF) != 0xFFFFFFF){
        EntrySize++;

        index = (Flip4Byte(entry) * 32) / 512 + 1;

        d->port->Read(6301 + index, 1, d->port->buffer);

        pos = Flip4Byte(entry);

        while (pos >= 16){
            pos -= 16;
        }

        GlobalRenderer->PrintAtCursor(to_string((uint64_t)pos));
        GlobalRenderer->PrintAtCursor(" - ");
        GlobalRenderer->PrintAtCursor(to_string((uint64_t)index));
        GlobalRenderer->PrintAtCursor(" - ");

        entry = Get4Byte(d->port->buffer, pos * 32);

        GlobalRenderer->PrintAtCursor(to_hstring((uint32_t)entry));
        GlobalRenderer->PrintAtCursor(" - ");
        GlobalRenderer->PrintAtCursor(to_string((double)Flip4Byte(entry)));
        GlobalRenderer->Next();
    }
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
    FILE::File file;

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