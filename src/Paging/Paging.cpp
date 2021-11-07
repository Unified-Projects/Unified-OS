#include <paging/paging.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Paging;

//Set flag for an entry
void PageDirectoryEntry::SetFlag(PT_Flag flag, bool enabled){
    //Get the Bit
    uint64_t bitSelector = (uint64_t)1 << flag;
    
    //Set to false
    Value &= ~bitSelector;

    //Set to true if needed
    if (enabled){
        Value |= bitSelector;
    }
}

//Get the entry flag
bool PageDirectoryEntry::GetFlag(PT_Flag flag){
    //Get the Bit
    uint64_t bitSelector = (uint64_t)1 << flag;

    //Return if it is true or false
    return Value & bitSelector > 0 ? true : false;
}

//Get entry address of Directory Entry
uint64_t PageDirectoryEntry::GetAddress(){
    return (Value & 0x000ffffffffff000) >> 12;
}

//Set entry address of Directory Entry
void PageDirectoryEntry::SetAddress(uint64_t address){
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
}
