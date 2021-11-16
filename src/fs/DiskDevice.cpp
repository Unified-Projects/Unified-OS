#include <fs/DiskDevice.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;

#include <paging/PageFrameAllocator.h>
#include <memory/memory.h>
#include <memory/heap.h>

#include <fs/GPT.h>

int DiskDevice::GetBuffer(){
    for(int i = 0; i < 8; i++){
        //Check if not locked
        if(!(bufferLock & (0b10000000 >> (i % 8)))){
            // bufferLock |= (1 << index); GET IT TO SET BIT AS FILLED
            return i;
        }
    }
    
    return -1;
}
int DiskDevice::RelieveBuffer(uint8_t index){
    bufferLock ^= (1 << index); //UPDATE TO FIX
    return 1;
}

DiskDevice::DiskDevice(){
    for(int i = 0; i < 8; i++){
        Buffer[i] = Paging::__PAGING__PFA_GLOBAL.RequestPage();
        Memory::memset(Buffer[i], 0x00, 0x1000);
    }

    bufferLock = 0x00;
}
#include <common/stdio.h>
#include <common/cstring.h>
int DiskDevice::InitializePartitions(){
    //First Check if using GPT or MBR (GPT Only supported)
    GPT::GPTHeader* Header = (GPT::GPTHeader*)(Memory::malloc(blocksize));

    //Read the GPT
    Read(blocksize, blocksize, (void*)Header);

    printf("LBA: ");
    printf(to_string((int64_t)Header->CurrentLBA));
    Next();

    printf("Size: ");
    printf(to_string((int64_t)Header->Size));
    Next();

    printf("PartitionTableLBA: ");
    printf(to_string((int64_t)Header->PartitionTableLBA));
    Next();

    printf("PartNum: ");
    printf(to_string((int64_t)Header->PartNum));
    Next();

    printf("PartEntrySize: ");
    printf(to_string((int64_t)Header->PartEntrySize));
    Next();
}

int DiskDevice::ReadDiskBlock(uint64_t lba, size_t count, void* buffer){}
int DiskDevice::WriteDiskBlock(uint64_t lba, size_t count, void* buffer){}

int DiskDevice::Read(size_t off, size_t size, uint8_t* buffer){
    //Find a temporary buffer to write to
    int BufferNumber = GetBuffer();
    
    //Ensure valid index
    if(BufferNumber == -1){
        return NULL;
    }

    //Get the buffer
    void* TempBuffer = Buffer[BufferNumber];

    //Work out starting LBA
    size_t StartLBA = off / blocksize;
    size_t OffsetInLBA = (off - ((off / blocksize) * blocksize));
    size_t SizeWithinFirstLBA = blocksize - OffsetInLBA;

    //Get that one block
    ReadDiskBlock(StartLBA, 1, Buffer[BufferNumber]);

    //Copy over the data of the First LBA
    Memory::memcpy(buffer, ((uint8_t*)Buffer[BufferNumber] + OffsetInLBA), SizeWithinFirstLBA);

    //Get the extra blocks
    size_t Blocks = (size - SizeWithinFirstLBA) / blocksize;
    size_t Excess = (size - SizeWithinFirstLBA) - (blocksize * Blocks);

    //Read all Blocks we can easily
    // if(Blocks > 0) //Stops any potential issues with Reading 0 blocks
    //     ReadDiskBlock(StartLBA + 1, Blocks, buffer + SizeWithinFirstLBA);

    for(int i = 0; i < Blocks; i++){
        if(Blocks - i > 8){
            ReadDiskBlock(StartLBA + 1 + i, 8, Buffer[BufferNumber]);
            Memory::memcpy(buffer + SizeWithinFirstLBA + (i * blocksize), Buffer[BufferNumber], blocksize * 8);

            i += 7;
        }
        else if(Blocks - i > 4){
            ReadDiskBlock(StartLBA + 1 + i, 4, Buffer[BufferNumber]);
            Memory::memcpy(buffer + SizeWithinFirstLBA + (i * blocksize), Buffer[BufferNumber], blocksize * 4);

            i += 3;
        }
        if(Blocks - i > 2){
            ReadDiskBlock(StartLBA + 1 + i, 2, Buffer[BufferNumber]);
            Memory::memcpy(buffer + SizeWithinFirstLBA + (i * blocksize), Buffer[BufferNumber], blocksize * 2);

            i++;
        }
        else{
            ReadDiskBlock(StartLBA + 1 + i, 1, Buffer[BufferNumber]);
            Memory::memcpy(buffer + SizeWithinFirstLBA + (i * blocksize), Buffer[BufferNumber], blocksize);
        }
    }

    //Read that last excess bit
    if(Excess > 0){ //Again potential issues
        //Modify that one block
        ReadDiskBlock(StartLBA + 1 + Blocks, 1, Buffer[BufferNumber]);

        //Copy over the data of the First LBA
        Memory::memcpy(buffer + SizeWithinFirstLBA + (Blocks * blocksize), Buffer[BufferNumber], Excess);
    }

    //Free Buffer
    RelieveBuffer(BufferNumber);

    return 1;
}
int DiskDevice::Write(size_t off, size_t size, uint8_t* buffer){
    //Find a temporary buffer to write to
    int BufferNumber = GetBuffer();
    
    //Ensure valid index
    if(BufferNumber == -1){
        return -1;
    }

    //Work out starting LBA
    size_t StartLBA = off / blocksize;
    size_t OffsetInLBA = (off - ((off / blocksize) * blocksize));
    size_t SizeWithinFirstLBA = blocksize - OffsetInLBA;

    //Modify that one block
    ReadDiskBlock(StartLBA, 1, Buffer[BufferNumber]);

    //Copy over the data of the First LBA
    Memory::memcpy(((uint8_t*)Buffer[BufferNumber] + OffsetInLBA), buffer, SizeWithinFirstLBA);

    //Write that modified Block
    WriteDiskBlock(StartLBA, 1, Buffer[BufferNumber]);

    //Get the extra blocks
    size_t Blocks = (size - SizeWithinFirstLBA) / blocksize;
    size_t Excess = (size - SizeWithinFirstLBA) - (blocksize * Blocks);

    //Write all Blocks we can easily
    // if(Blocks > 0) //Stops any potential issues with writing 0 blocks
    //     WriteDiskBlock(StartLBA + 1, Blocks, (buffer + SizeWithinFirstLBA));
    for(int i = 0; i < Blocks; i++){
        if(Blocks - i > 8){
            Memory::memcpy(Buffer[BufferNumber], buffer + SizeWithinFirstLBA + (i * blocksize), blocksize * 8);
            WriteDiskBlock(StartLBA + 1 + i, 8, Buffer[BufferNumber]);

            i += 7;
        }
        else if(Blocks - i > 4){
            Memory::memcpy(Buffer[BufferNumber], buffer + SizeWithinFirstLBA + (i * blocksize), blocksize * 4);
            WriteDiskBlock(StartLBA + 1 + i, 4, Buffer[BufferNumber]);

            i += 3;
        }
        if(Blocks - i > 2){
            Memory::memcpy(Buffer[BufferNumber], buffer + SizeWithinFirstLBA + (i * blocksize), blocksize * 2);
            WriteDiskBlock(StartLBA + 1 + i, 2, Buffer[BufferNumber]);

            i++;
        }
        else{
            Memory::memcpy(Buffer[BufferNumber], buffer + SizeWithinFirstLBA + (i * blocksize), blocksize);
            WriteDiskBlock(StartLBA + 1 + i, 1, Buffer[BufferNumber]);
        }
    }

    //Write that last excess bit
    if(Excess > 0){ //Again potential issues
        //Modify that one block
        ReadDiskBlock(StartLBA + 1 + Blocks, 1, Buffer[BufferNumber]);

        //Copy over the data of the First LBA
        Memory::memcpy(Buffer[BufferNumber], buffer, Excess);

        //Write that modified Block
        WriteDiskBlock(StartLBA + 1 + Blocks, 1, Buffer[BufferNumber]);
    }
    
    //Relieve the buffer
    RelieveBuffer(BufferNumber);

    return 1;
}

DiskDevice::~DiskDevice(){

}

void DiskDevice::SetName(const char* name){
    DeviceName = (char*)name;
}