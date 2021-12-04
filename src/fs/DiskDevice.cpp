#include <fs/DiskDevice.h>

using namespace UnifiedOS;
using namespace UnifiedOS::FileSystem;

#include <paging/PageFrameAllocator.h>
#include <memory/memory.h>
#include <memory/heap.h>

#include <fs/GPT.h>
#include <fs/MBR.h>

#include <fs/Formats/FAT/FAT32/FATPartition.h>

#include <fs/ByteInteraction.h>

#include <fs/VolumeManager.h>

//Disk Type Checker
PartitionDevice* GPTCreatePartition(GPT::GPTPartitonEntry Entry, DiskDevice* diskDev){
    //GetType
    GPT::GUID Guid;
    Guid.operator=((uint8_t*)&(Entry.TypeGUID));

    GPT::GPT_Types PartType =  Guid.GetGUIDType();

    //Switch Type
    switch (PartType)
    {
    case GPT::BASIC_DATA_PARITITON :{//The "FAT" Types
        //Boot Sector
        void* BootSector = Memory::malloc(512);
        diskDev->ReadBlocks(Entry.StartLBA, 1, BootSector);

        switch (UnifiedOS::FileSystem::Byte::Get8Byte(BootSector, 82)) //FATs
        {
        case 0x4641543332202020: //FAT32
            return new FAT32::Fat32Partition(Entry, diskDev, BootSector);
            break;
        
        default:
            break;
        }

        //Free boot sector
        Memory::free(BootSector);

        break;
        }
    case GPT::EFI_SYSTEM_PARTITION: //Same as ^
        break;
    }

    //No FS Found or compatible
    return nullptr;
}

//Recive one of 8 buffers
int DiskDevice::GetBuffer(){
    for(int i = 0; i < 8; i++){
        //Check if not locked
        if(!((bufferLock >> i) & 1U)){
            // bufferLock ^= (-1 ^ bufferLock) & (1UL << i);
            bufferLock |= (1<<i);
            return i;
        }
    }
    
    return -1;
}
int DiskDevice::RelieveBuffer(uint8_t index){
    // bufferLock ^= (-1 ^ bufferLock) & (1UL << index); //Toggle bit to off
    bufferLock &= ~(1<<index);
    return 1;
}

DiskDevice::DiskDevice(){
    for(int i = 0; i < 8; i++){
        //Setup the buffer
        Buffer[i] = Paging::__PAGING__PFA_GLOBAL.RequestPage();
        Memory::memset(Buffer[i], 0x00, 0x1000);
    }

    bufferLock = 0x00;
}

int DiskDevice::InitializePartitions(){
    //First Check if using GPT or MBR (GPT Only supported)
    GPT::GPTHeader* Header = (GPT::GPTHeader*)(Memory::malloc(blocksize));

    //Read the GPT
    ReadBlocks(1, 1, Header);

    if(Header->Signature == GPT_HEADER_SIGNATURE_BIG_ENDIAN){
        //Read partition Entries
        GPT::GPTPartitonEntry* Partitions = (GPT::GPTPartitonEntry*)Memory::malloc(Header->PartEntrySize * Header->PartNum);
        ReadBlocks(Header->PartitionTableLBA, (Header->PartNum * Header->PartEntrySize)/blocksize, Partitions);

        for(int i = 0; i < Header->PartNum; i++){
            if(Partitions[i].TypeGUID[0] != 0x00){
                //Create a partition
                PartitionDevice* partitionOut = GPTCreatePartition(Partitions[i], this);

                if(partitionOut == nullptr){
                    continue;
                }

                //Add it to list
                partitions.add_back(partitionOut);
            
                //Next Mount it
                if(partitionOut->Flags.AutoMount && !partitionOut->Flags.Hidden){
                    __FS_VOLUME_MANAGER__->MountPartition(partitionOut);
                }
            }
        }

        Memory::free(Partitions);
    }
    else{
        return -1; //MBR not implemented
    }

    Memory::free(Header);
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
    size_t OffsetInLBA = (off % blocksize);
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

int DiskDevice::ReadBlocks(uint64_t lba, size_t count, void* buffer){
    uint64_t lbapos = 0;

    //Get a buffer
    uint8_t BufNum = GetBuffer();

    //Exit on fail
    if(BufNum == -1){
        return -1;
    } 

    //Read in block sizes of 8 (Fast)
    for(; (count-lbapos >= 8); lbapos+=8){
        ReadDiskBlock(lba + lbapos, 8, Buffer[BufNum]);
        Memory::memcpy(((uint8_t*)buffer + (lbapos * 512)), Buffer[BufNum], blocksize*8);
    }

    //Read in block size of 4
    for(; (count-lbapos >= 4); lbapos+=4){
        ReadDiskBlock(lba + lbapos, 4, Buffer[BufNum]);
        Memory::memcpy(((uint8_t*)buffer + (lbapos * 512)), Buffer[BufNum], blocksize*4);
    }

    //Block size of 2
    for(; (count-lbapos >= 2); lbapos+=2){
        ReadDiskBlock(lba + lbapos, 2, Buffer[BufNum]);
        Memory::memcpy(((uint8_t*)buffer + (lbapos * 512)), Buffer[BufNum], blocksize*2);
    }

    //Finish of the blocks
    for(; (count-lbapos > 0); lbapos++){
        ReadDiskBlock(lba + lbapos, 1, Buffer[BufNum]);
        Memory::memcpy(((uint8_t*)buffer + (lbapos * 512)), Buffer[BufNum], blocksize);
    }
}
int DiskDevice::WriteBlocks(uint64_t lba, size_t count, void* buffer){
    uint64_t lbapos = 0;

    //Buffer
    uint8_t BufNum = GetBuffer();

    if(BufNum == -1){
        return -1;
    }

    //Read blocks in sections
    for(; (count-lbapos >= 8); lbapos+=8){
        Memory::memcpy(Buffer[BufNum], ((uint8_t*)buffer + (lbapos * 512)), blocksize*8);
        WriteDiskBlock(lba + lbapos, 8, Buffer[BufNum]);
    }

    for(; (count-lbapos >= 4); lbapos+=4){
        Memory::memcpy(Buffer[BufNum], ((uint8_t*)buffer + (lbapos * 512)), blocksize*4);
        WriteDiskBlock(lba + lbapos, 4, Buffer[BufNum]);
    }

    for(; (count-lbapos >= 2); lbapos+=2){
        Memory::memcpy(Buffer[BufNum], ((uint8_t*)buffer + (lbapos * 512)), blocksize*2);
        WriteDiskBlock(lba + lbapos, 2, Buffer[BufNum]);
    }

    //End blocks
    for(; (count-lbapos > 0); lbapos++){
        Memory::memcpy(Buffer[BufNum], ((uint8_t*)buffer + (lbapos * 512)), blocksize);
        WriteDiskBlock(lba + lbapos, 1, Buffer[BufNum]);
    }
}

DiskDevice::~DiskDevice(){

}

void DiskDevice::SetName(const char* name){
    DeviceName = (char*)name;
}