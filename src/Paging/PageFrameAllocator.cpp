#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialised = false;
PageFrameAllocator GlobalAllocator;

//Load memeory from boot loader
void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t DescriptorSize){
    //Run once
    if(Initialised) return;
    Initialised = true;

    //Entry count
    uint64_t mMapEntries = mMapSize / DescriptorSize;

    //Segmenting
    void* largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;

    //Loop over entries and find largest
    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * DescriptorSize));
        if(desc->type == 7){ //EfiConventionalMemory
            if(desc->numPages * 4096 > largestFreeMemSegSize){
                largestFreeMemSeg = desc->physAddr;
                largestFreeMemSegSize = desc->numPages * 4096;
            }
        }
    }

    //Get total size
    uint64_t memorySize = GetMemorySize(mMap, mMapEntries, DescriptorSize);
    freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;

    //Load 
    InitBitmap(bitmapSize, largestFreeMemSeg);

    //Lock all Pages
    ReservePages(0, memorySize / 4096 + 1);
    
    //If Loop over entries and unlock if its Conventional (Usable) memory
    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * DescriptorSize));
        if(desc->type == 7){ //EfiConventionalMemory
            UnreservePages(desc->physAddr, desc->numPages);
        }
    }
    //Lock Bios Pages
    ReservePages(0, 0x100); //Reserver 0 - 0x100000 (BIOS)
    //Lock Bitmap Pages
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
}

//Load bitmap at size of pages
void PageFrameAllocator::InitBitmap(size_t bitMapSize, void* bufferAddress){
    PageBitmap.Size = bitMapSize;
    PageBitmap.Buffer = (uint8_t*)bufferAddress;
    for(int i = 0; i < bitMapSize; i++){
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}

//Request a single page (returned)
uint64_t pageBitmapIndex = 0;
void* PageFrameAllocator::RequestPage(){
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++){
        if (PageBitmap[pageBitmapIndex] == true) continue;
        LockPage((void*)(pageBitmapIndex * 4096));
        return (void*)(pageBitmapIndex * 4096);
    }

    return NULL; // Page Frame Swap to file
}

//Request Multiple Pages (returned)
void* PageFrameAllocator::RequestPages(uint64_t count){
    uint64_t BackupIndex = pageBitmapIndex;
    bool Found = true;
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++){
        Found = true;
        for(int i = 0; i < count; i++){
            if (PageBitmap[pageBitmapIndex + i] == true) {Found = false;continue;}
        }

        if(Found == true){
            LockPages((void*)(pageBitmapIndex * 4096), count);
            return (void*)(pageBitmapIndex * 4096);
        }
    }

    pageBitmapIndex = BackupIndex;
    return NULL;
}

//Unlock a page
void PageFrameAllocator::FreePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if(PageBitmap[index] == false) return;
    if (PageBitmap.Set(index, false)){
        freeMemory += 4096;
        usedMemory -= 4096;
        if(pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

//Unlock many pages
void PageFrameAllocator::FreePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        FreePage((void*)((uint64_t)address + (t * 4096)));
    }
}

//Lock a page (At given Address)
void PageFrameAllocator::LockPage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if(PageBitmap[index] == true) return;
    if(PageBitmap.Set(index, true)){
        freeMemory -= 4096;
        usedMemory += 4096;
    }
}

//Lock pages (At given Address)
void PageFrameAllocator::LockPages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        LockPage((void*)((uint64_t)address + (t * 4096)));
    }
}

//Get Rid Of Free Memory (At address)
void PageFrameAllocator::ReservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if(PageBitmap[index] == true) return;
    if(PageBitmap.Set(index, true)){
        freeMemory -= 4096;
        reservedMemory += 4096;
    }   
}

//Get Rid Of Free Memory (At address)
void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        ReservePage((void*)((uint64_t)address + (t * 4096)));
    }
}

//Return Reserved Memory To Free Memory (Unlocking Entry)
void PageFrameAllocator::UnreservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if(PageBitmap[index] == false) return;
    if(PageBitmap.Set(index, false)){
        freeMemory += 4096;
        reservedMemory -= 4096;
        if(pageBitmapIndex > index) pageBitmapIndex = index;
    }
}

//Return Reserved Memory To Free Memory (Unlocking Entry)
void PageFrameAllocator::UnreservePages(void* address, uint64_t pageCount){
    for (int t = 0; t < pageCount; t++){
        UnreservePage((void*)((uint64_t)address + (t * 4096)));
    }
} 

//Get RamFree Memory Size (Bytes)
uint64_t PageFrameAllocator::GetFreeRAM(){
    return freeMemory;
}

//Get Uses Memory Size (Bytes)
uint64_t PageFrameAllocator::GetUsedRAM(){
    return usedMemory;
}

//Get Reserved Memory Size (Bytes)
uint64_t PageFrameAllocator::GetReservedRAM(){
    return reservedMemory;
}