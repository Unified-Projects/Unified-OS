#include "DiskReading.h"
#include "ByteInteraction.h"
#include "../memory/heap.h"
#include "../Paging/PageFrameAllocator.h"

//Splits the 32Bit entry into all sections of data it stands for
ROOT_DIRECTORY_ENTRY GetRDE(void* buffer, uint64_t EntryOffset){
    //For Returning
    ROOT_DIRECTORY_ENTRY rde = ROOT_DIRECTORY_ENTRY{};

    //Allocation Status
    rde.AllocationStatus = GetByte(buffer, EntryOffset);

    //File Name (shortened if LFN and all lower case (So we offset a bit))
    for (int i = 0; i < 11; i++){
        if(GetByte(buffer, EntryOffset + i) >= 65 && GetByte(buffer, EntryOffset + i) <= 90){
            rde.FileName[i] = GetByte(buffer, EntryOffset + i) + 0x20;
        }
        else{
            rde.FileName[i] = GetByte(buffer, EntryOffset + i);
        }
    }
    
    //File Attributes
    rde.FileAttributes.Set(GetByte(buffer, EntryOffset + 11));

    //Unused
    rde.Reserved = GetByte(buffer, EntryOffset + 12);

    //Tenth of seconds (Not used by DOS)
    rde.FileCreationTime = GetByte(buffer, EntryOffset + 13);

    //Creation Time hours minutes seconds
    rde.CreationTime = Get4Byte(buffer, EntryOffset + 14);

    //Creation Date (Not used by DOS)
    rde.CreationDate = Get2Byte(buffer, EntryOffset + 16);

    //Access Data (Not used by DOS)
    rde.AccessDate = Get2Byte(buffer, EntryOffset + 18);

    //High two bytes of cluster address
    rde.High2BytesOfAddressOfFirstCluster = Get2Byte(buffer, EntryOffset + 20);

    //Modified Time hours minutes seconds
    rde.ModifiedTime = Get2Byte(buffer, EntryOffset + 22);

    //Modified Date
    rde.ModifiedDate = Get2Byte(buffer, EntryOffset + 24);

    //Low Two Bytes of cluster
    rde.Low2BytesOfAddressOfFirstCluster = GetByte(buffer, EntryOffset + 26);

    //File Size (Flipped For Actual Value) (Will be zero if not file)
    rde.FileSize = Flip4Byte(Get4Byte(buffer, EntryOffset + 28));

    //Returns
    return rde;
}

//Splits the 32 bits into a Long File Name Entry
LFN_DRIECTORY_ENTRY GetLFNDE(void* buffer, uint64_t EntryOffset){
    //To return
    LFN_DRIECTORY_ENTRY lfne = LFN_DRIECTORY_ENTRY{};

    //Character Indexing
    uint16_t charIndex = 0;

    //Sequence Number
    lfne.SequenceNumberAndAllocationStatus = GetByte(buffer, EntryOffset);

    //File Name Part 1
    //We Use Skip To Ignore unreadable Characters
    bool Skip = false;
    for (int i = 0; i < 10; i++){
        if(!Skip){
            lfne.FileName[charIndex] = GetByte(buffer, EntryOffset + 1 + i);
            charIndex++;
        }
        Skip = !Skip;
    }

    //File attributes
    lfne.FileAttributes.Set(GetByte(buffer, EntryOffset + 11));

    //Unused
    lfne.Reserved = GetByte(buffer, EntryOffset + 12);

    //Checking and validation
    lfne.Checksum = GetByte(buffer, EntryOffset + 13);

    //File Name Part 2
    //We Use Skip To Ignore unreadable Characters
    Skip = false;
    for (int i = 0; i < 12; i++){
        if(!Skip){
            lfne.FileName[charIndex] = GetByte(buffer, EntryOffset + 14 + i);
            charIndex++;
        }
        Skip = !Skip;
    }

    //Unused2
    lfne.Reserved2 = Get2Byte(buffer, EntryOffset + 26);

    //File Name Part 3
    //We Use Skip To Ignore unreadable Characters
    Skip = false;
    for (int i = 0; i < 4; i++){
        if(!Skip){
            lfne.FileName[charIndex] = GetByte(buffer, EntryOffset + 28 + i);
            charIndex++;
        }
        Skip = !Skip;
    }

    //Return
    return lfne;
}

//Checks if the buffer entry is a long file name entry
bool isLFN(void* buffer, uint64_t EntryOffset){
    return (GetByte(buffer, EntryOffset + 11) == 0xF);
}

//Checks if it is a valid entry
bool ValidateEntry(void* buffer, uint64_t EntryOffset){
    if(GetByte(buffer, EntryOffset) == 0x00) return false; //Empty
    if(GetByte(buffer, EntryOffset) == 0x05) return false; //Pending Delete
    //if(GetByte(buffer, EntryOffset) == 0x2E) return false; //Dot (.. or .)
    if(GetByte(buffer, EntryOffset) == 0xE5) return false; //Deleted

    return true;
}

//Reads the buffer for the entry also getting the longer file names
DIRECTORY_ENTRY RootGetDirectoryEntry(void* buffer, uint64_t EntryOffset, uint32_t rootStart){
    //Check if ValidEntry
    if(!ValidateEntry(buffer, EntryOffset)){
        return DIRECTORY_ENTRY{};
    }
    
    //Return value
    DIRECTORY_ENTRY entry = DIRECTORY_ENTRY{};

    //For entry checking
    entry.Valid = true;

    //First Check if LFN
    if(isLFN(buffer, EntryOffset)){
        //Offseting and indexing
        uint32_t CurrentLFNOffset = 0;
        uint8_t LFNIndex = 15;
        uint8_t LFNSize = 0;

        //Loop over all LFNs
        while(isLFN(buffer, EntryOffset + CurrentLFNOffset)){
            
            //Get data
            entry.LFNE[LFNIndex--] = GetLFNDE(buffer, EntryOffset + CurrentLFNOffset); 

            //Offset increase
            CurrentLFNOffset += 32;

            //Size increase
            LFNSize += 1;

            //Stop if at limit
            if(LFNIndex == 0){
                break;
            }
        }

        //Set Size
        entry.LFNEntries = LFNSize;

        //Get actual entry
        entry.RDE = GetRDE(buffer, EntryOffset + CurrentLFNOffset);
    }
    //Otherwise act normal
    else{
        //Read in the directory entry
        entry.RDE = GetRDE(buffer, EntryOffset);
    }

    if(entry.RDE.FileSize > 0){
        //File Segment
        uint32_t FClusterH = ((uint32_t)entry.RDE.High2BytesOfAddressOfFirstCluster) << 16;
        uint32_t FClusterL = ((uint32_t)entry.RDE.Low2BytesOfAddressOfFirstCluster);
        uint32_t FCluster = (FClusterH & 0xFFFF0000) | (FClusterL & 0x0000FFFF);

        entry.FILE_FIRST_SEGMENT = FCluster + rootStart - 2;
    }

    //Return result
    return entry;
}
