#include "heap.h"
#include "../Paging/PageTableManager.h"
#include "../Paging/PageFrameAllocator.h"

void* heapStart;
void* heapEnd;
HeapSegHdr* LastHdr;

//Join two segments
void HeapSegHdr::CombineForward(){
    //If Exists and is free
    if (next == NULL) return;
    if (!next->free) return;

    //Configure Header(References)
    if (next == LastHdr) LastHdr = this;

    //Refereences
    if (next->next != NULL){
        next->next->last = this;
    }

    //Join Sizes
    length = length + next->length + sizeof(HeapSegHdr);

    //More References
    next = next->next;
}   

//Combining
void HeapSegHdr::CombineBackward(){
    if (last != NULL && last->free) last->CombineForward();
}

//Separate one segment into 2 (return one of defined size)
HeapSegHdr* HeapSegHdr::Split(size_t splitLength){
    //Ensure it is long enough
    if(splitLength < 0x10) return NULL;
    //Get the other size
    int64_t splitSegLength = length - splitLength - (sizeof(HeapSegHdr));
    //Ensure the other segment is big enough
    if(splitSegLength < 0x10) return NULL;

    //Split
    HeapSegHdr* newSplitHdr = (HeapSegHdr*)((size_t)this + splitLength + sizeof(HeapSegHdr));
    next->last = newSplitHdr; //Set next segments last to new
    newSplitHdr->next = next; //Set new Segments next to the original
    next = newSplitHdr; //Set new next Segment to the the new one
    newSplitHdr->last = this; //Set this os the last for the new segment
    newSplitHdr->length = splitSegLength; //Set headers length to calculated
    newSplitHdr->free = free; //Make sure the new segments free (same as the original)
    length = splitLength; //Set the new length of original to what it should be after split

    //Sort references then return
    if(LastHdr == this) LastHdr = newSplitHdr;
    return newSplitHdr;
}

//Heap setup
void InitializeHeap(void* heapAddress, size_t pageCount){
    //(From Intitialise Kernel)
    void* pos = heapAddress; 

    //Get Pages from Allocator
    for(size_t i = 0; i < pageCount; i++){
        GlobalPTM.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }

    //Get Size of intial Heap
    size_t heapLength = pageCount * 0x1000;

    //Setup first segment
    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapSegHdr* startSeg = (HeapSegHdr*)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegHdr);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    LastHdr = startSeg;
}

//Allocate memory from heap
void* malloc(size_t size){
    if(size % 0x10 > 0) { //not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    //If Not a valid size
    if(size <= 0) return NULL;

    //Load over segments sorting heap
    HeapSegHdr* currentSeg = (HeapSegHdr*)heapStart;
    while(true){
        if(currentSeg->free){
            if(currentSeg->length > size){
                currentSeg->Split(size);
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
            }
            if(currentSeg->length == size){
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
            }
        }
        if(currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }

    //Increase size of heap
    ExpandHeap(size);
    return malloc(size);
}

//Free a specific heap
void free(void* address){
    HeapSegHdr* segment = (HeapSegHdr*)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

//Add more memory to heap
void ExpandHeap(size_t length){
    //Ensure pages big
    if (length % 0x1000) {
        length -= length % 0x1000;
        length += 0x1000;
    }

    //Get pages count
    size_t pageCount = length / 0x1000;
    //Make a new segment
    HeapSegHdr* newSegment = (HeapSegHdr*)heapEnd;

    //Load size onto segment
    for (size_t i = 0; i < pageCount; i++){
        GlobalPTM.MapMemory(heapEnd, GlobalAllocator.RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000);
    }

    //Setup segment
    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next = newSegment;
    LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegHdr);
    newSegment->CombineBackward();
}