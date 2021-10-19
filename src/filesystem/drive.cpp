#include "drive.h"
#include "../screen/rendering/BasicRenderer.h"
#include "../utils/cstr.h"
#include "../paging/PageFrameAllocator.h"

#include "./ByteInteraction.h"

//Basic Global drives for all to use
Drive* Drives[16];

//And the count to ensure no null referencing
uint8_t DriveCount = 0;

//Main intialiser
Drive::Drive(AHCI::Port* portPtr){
    //Set its drive port
    this->port = portPtr;

    //Clear the buffer
    memset(port->buffer, 0, 0x1000);

    //Read MBR Sector Into Buffer
    port->Read(0, 1, port->buffer);

    //Read MBR Data
    DriveMBR = ReadMBR(port->buffer, DriveMBR);

    //Check MBR
    if(DriveMBR->BootRecordSignature == 0x55AA)
        //Set Drive Type Accordingly
        DriveType = MASTER_BOOT_RECORD;

    //Read What would be the GPT Sector Into Buffer
    port->Read(1, 1, port->buffer);

    //Check GPT
    DriveGPT = ReadGPT(port->buffer, DriveGPT);

    //If GPT is found
    if(strcmp(DriveGPT->Signature, "EFI PART", 8))
        //Set Drive Type Accordingly
        DriveType = GUID_PARTITION_TABLE;

    //Read GPT Partitions if GPT is found
    if(DriveType == GUID_PARTITION_TABLE){
        
        //Read Off GPT Partitions
        DriveGPT = ReadGPTParitions(port, DriveGPT);

        //Setup Partitions
        Partitions = new Partition[DriveGPT->EntryCount];

        //Load Partitions
        for(int i = 0; i < DriveGPT->EntryCount; i++){
            if(!DriveGPT->Entries[i].Empty){
                //Load MBR
                port->Read(DriveGPT->Entries[i].StartingLBA, 1, port->buffer);
                Partitions[i].PartitionMBR = ReadMBR(port->buffer, Partitions[i].PartitionMBR);
                
                //Initiate (Work Out Format)
                Partitions[i].Init(&DriveGPT->Entries[i]);
            }
        }
    }
}