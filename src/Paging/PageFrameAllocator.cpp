#include <paging/PageFrameAllocator.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Paging;
using namespace UnifiedOS::Boot;

static uint64_t FreeMemory;
static uint64_t ReservedMemory;
static uint64_t UsedMemory;
static uint64_t TotalMemory;

bool Initialised = false;
PageFrameAllocator UnifiedOS::Paging::__PAGING__PFA_GLOBAL;

const uint64_t& UnifiedOS::Paging::__PAGING__TotalMemorySize__ = TotalMemory;

//Returns the Size Of Memory from the UEFI
uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t DescriptorSize){
    uint64_t MemorySizeInBytes = 0;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * DescriptorSize));
        MemorySizeInBytes += desc->numPages * 4096;
    }

    TotalMemory = MemorySizeInBytes;

    return MemorySizeInBytes;
}

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t DescriptorSize){
    //Single Use
    if(Initialised == true) return;
    Initialised = true;

    //Calculate Entries
    uint64_t MapEntries = mMapSize / DescriptorSize;

    //Largest Free Segment (For bitmap to be stored on (Not whole size though))
    void* LargestSegment = nullptr;
    size_t LargestSegmentSize = 0;

    //Loop over entries
    for(int e = 0; e < MapEntries; e++){
        EFI_MEMORY_DESCRIPTOR* Descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (e * DescriptorSize));
        
        //Only Load Conventional (Free) Memory
        if(Descriptor->type == 7){
            if(Descriptor->numPages * 4096 > LargestSegmentSize){
                LargestSegment = Descriptor->physAddr;
                LargestSegmentSize = Descriptor->numPages * 4096;
            }
        }
    }

    //Get the Total Size Of Memory
    uint64_t MemorySize = GetMemorySize(mMap, MapEntries, DescriptorSize);
    
    //Call the memory all to be free (It is not but that gets fixed)
    FreeMemory = MemorySize;

    //Calculate how many Pages there are to be in the Bitmap
    uint64_t BitmapSize = ((MemorySize / 4096) / 8) + 1;

    //Load The Bitmap
    InitBitmap(BitmapSize, LargestSegment);

    //Class all pages as locked
    ReservePages(0, MemorySize / 4096 + 1);

    //Loop over any pages that are free (Same As Above)
    for(int e = 0; e < MapEntries; e++){
        EFI_MEMORY_DESCRIPTOR* Descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (e * DescriptorSize));
        
        //Only Load Conventional (Free) Memory
        if(Descriptor->type == 7){
            UnreservePages(Descriptor->physAddr, Descriptor->numPages);
        }
    }

    //Lock the BIOS pages
    ReservePages((void*)0, 0x100);

    //Lock the Bitmap
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
}

//Create a Bitmap of the pages
void PageFrameAllocator::InitBitmap(size_t bitMapSize, void* bufferAddress){
    //Setup Bitmap Definitions
    PageBitmap.Size = bitMapSize;
    PageBitmap.Buffer = (uint8_t*)bufferAddress;

    //Loop over size
    for(int b = 0; b < bitMapSize; b++){
        *(uint8_t*)(PageBitmap.Buffer + b) = 0; //Set to Free (False)
    }
}

//
//
//
//
// NOTE CREATE PAGE FILES (TO STOP PAGE FAULTS)
//
//
//
//


//Request a single page (returned)
uint64_t pageBitmapIndex = 0;
void* PageFrameAllocator::RequestPage(){
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++){
        if (PageBitmap[pageBitmapIndex] == true) continue;
        LockPage((void*)(pageBitmapIndex * 4096));
        return (void*)(pageBitmapIndex * 4096);
    }

    return NULL; // Page Frame Swap file to be implemented
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
        FreeMemory += 4096;
        UsedMemory -= 4096;
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
        FreeMemory -= 4096;
        UsedMemory += 4096;
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
        FreeMemory -= 4096;
        ReservedMemory += 4096;
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
        FreeMemory += 4096;
        ReservedMemory -= 4096;
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
    return FreeMemory;
}

//Get Uses Memory Size (Bytes)
uint64_t PageFrameAllocator::GetUsedRAM(){
    return UsedMemory;
}

//Get Reserved Memory Size (Bytes)
uint64_t PageFrameAllocator::GetReservedRAM(){
    return ReservedMemory;
}