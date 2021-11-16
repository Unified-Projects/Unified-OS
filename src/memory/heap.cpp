#include <memory/heap.h>
#include <paging/PageFrameAllocator.h>
#include <paging/PageTableManager.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Memory;
using namespace UnifiedOS::Paging;

void* heapStart;
void* heapEnd;
HeapBlockHeader* LastHdr;

//Join two segments (Hard to word comments)
void HeapBlockHeader::CombineForward(){
    //Check if there is a next to combine with
    if (Next == nullptr) return;

    //Ensure it is free
    if (!Next->Free) return;

    //If next is the final header in the heap then assign to current
    if (Next == LastHdr) LastHdr = this;

    //If the next had a next then configure it to this
    if (Next->Next != nullptr){
        Next->Next->Prev = this;
    }

    //Combine the two sizes
    Length = Length + Next->Length + sizeof(HeapBlockHeader);

    //Update this next to the actuall next position
    Next = Next->Next;
}

//Combine with the block behind
void HeapBlockHeader::CombineBackward(){
    //If the previous is free then just repeat combine forward for it
    if (Prev != nullptr && Prev->Free) Prev->CombineForward();
}

//Separate one segment into 2 (return one of defined size)
HeapBlockHeader* HeapBlockHeader::Split(size_t splitLength){
    //Ensure it is long enough
    if(splitLength < 0x10) return NULL;
    //Get the other size
    int64_t splitSegLength = Length - splitLength - (sizeof(HeapBlockHeader));
    //Ensure the other segment is big enough
    if(splitSegLength < 0x10) return NULL;

    //Split
    HeapBlockHeader* newSplitHdr = (HeapBlockHeader*)((size_t)this + splitLength + sizeof(HeapBlockHeader));
    Next->Prev = newSplitHdr; //Set next segments last to new
    newSplitHdr->Next = Next; //Set new Segments next to the original
    Next = newSplitHdr; //Set new next Segment to the the new one
    newSplitHdr->Prev = this; //Set this os the last for the new segment
    newSplitHdr->Length = splitSegLength; //Set headers length to calculated
    newSplitHdr->Free = Free; //Make sure the new segments free (same as the original)
    Length = splitLength; //Set the new length of original to what it should be after split

    //Sort references then return
    if(LastHdr == this) LastHdr = newSplitHdr;
    return newSplitHdr;
}

//Heap setup
void Memory::InitialiseHeap(void* heapAddress, size_t pageCount){
    //(From Intitialise Kernel)
    void* pos = heapAddress; 

    //Get Pages from Allocator
    for(size_t i = 0; i < pageCount; i++){
        //Map them
        __PAGING__PTM_GLOBAL.MapMemory(pos, __PAGING__PFA_GLOBAL.RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }

    //Get Size of intial Heap
    size_t heapLength = pageCount * 0x1000;

    //Setup first segment to use whole heap
    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapBlockHeader* startSeg = (HeapBlockHeader*)heapAddress;
    startSeg->Length = heapLength - (size_t)sizeof(HeapBlockHeader);
    startSeg->Next = nullptr;
    startSeg->Prev = nullptr;
    startSeg->Free = true;
    LastHdr = startSeg;
}

//Allocate memory from heap
void* Memory::malloc(size_t size){
    //Make sure a valid size
    if (size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }
    
    //End if invalid
    if (size == 0) return nullptr;

    //Create segment pointer to the first block
    HeapBlockHeader* CurrentSegment = (HeapBlockHeader*)heapStart;
    while(true){
        //Move allong heap untill heap found with correct size
        if(CurrentSegment->Free){
            if (CurrentSegment->Length > size){
                CurrentSegment->Split(size);
                CurrentSegment->Free = false;
                return (void*)((uint64_t)CurrentSegment + sizeof(HeapBlockHeader));
            }
            if (CurrentSegment->Length == size){
                CurrentSegment->Free = false;
                return (void*)((uint64_t)CurrentSegment + sizeof(HeapBlockHeader));
            }
        }
        if (CurrentSegment->Next == NULL) break;
        CurrentSegment = CurrentSegment->Next;
    }

    //Add more memory to heap
    ExpandHeap(size);

    //Continue until block found
    return malloc(size);
}

//Free a specific heap
void Memory::free(void* address){
    //Calculate where the segment is
    HeapBlockHeader* segment = (HeapBlockHeader*)address - 1;

    //Free it
    segment->Free = true;

    //Combine with neighbours if possible
    segment->CombineForward();
    segment->CombineBackward();
}

//Add more memory to heap
void Memory::ExpandHeap(size_t length){
    //Ensure pages fit 4KiB
    if (length % 0x1000) {
        length -= length % 0x1000;
        length += 0x1000;
    }

    //Get pages count
    size_t pageCount = length / 0x1000;

    //Attach to end of heap
    HeapBlockHeader* newSegment = (HeapBlockHeader*)heapEnd;

    //Load size onto segment
    for (size_t i = 0; i < pageCount; i++){
        void* Page = __PAGING__PFA_GLOBAL.RequestPage();
        if(Page != NULL){
            __PAGING__PTM_GLOBAL.MapMemory(heapEnd, Page);
            heapEnd = (void*)((size_t)heapEnd + 0x1000);
        }
    }

    //Setup segment
    newSegment->Free = true;
    newSegment->Prev = LastHdr;
    LastHdr->Next = newSegment;
    LastHdr = newSegment;
    newSegment->Next = nullptr;
    newSegment->Length = length - sizeof(HeapBlockHeader);
    newSegment->CombineBackward();
}