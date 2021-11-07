#ifndef __UNIFIED_OS_MEMORY_HEAP_H
#define __UNIFIED_OS_MEMORY_HEAP_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Memory{
        struct HeapBlockHeader{
            //Memory Size (Bytes)
            size_t Length;
            
            //References for Combine Forward / Backward
            HeapBlockHeader* Next;
            HeapBlockHeader* Prev;

            //If allocated already
            bool Free;

            //Once freed we combine to increase size and stop so many splits
            void CombineForward();
            void CombineBackward();

            //Separate to required size
            HeapBlockHeader* Split(size_t splitLength);
        };

        //Only to be ran one. Creates a heap using the PFA
        //
        // IF NO BOOT TRY TO INCREASE INTITIAL PAGE COUNT
        // IT IS KNOWN TO CAUSE ISSUES IF TOO LOW
        // DEFAULT = 0xFF
        //
        void InitialiseHeap(void* heapAddress, size_t pageCount);

        //Will get pages from the PFA
        void ExpandHeap(size_t lenght);

        void* malloc(size_t size); //Returns a buffer address for memory
        inline void* realloc(void* address) {return address;} // Will return same size
        void free(void* address); //Sets the heap block header to free
    }
}

//Operators for when creating a "new" pointer
inline void* operator new(size_t size) {return UnifiedOS::Memory::malloc(size);}
inline void* operator new[](size_t size) {return UnifiedOS::Memory::malloc(size);} // (new char[123] will return a char* with size of 123)

//Acts as a deleter for pointers
inline void operator delete(void* p) {UnifiedOS::Memory::free(p);}

#endif