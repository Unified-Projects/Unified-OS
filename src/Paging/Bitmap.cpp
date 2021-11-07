#include <paging/bitmap.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Paging;

//Custom Indexing from Bitmap (Read Only Form)
bool Bitmap::operator[](uint64_t index){
    Get(index);
}

//Get a set flag from the bitmap
bool Bitmap::Get(uint64_t index){
    //Validate Index
    if(index > Size * 8) return false;

    //Get the index of the byte
    uint64_t byteIndex = index / 8;

    //Get the index of the Bit
    uint8_t bitIndex = index % 8;

    //Calculate the Mask (Bitwise And)
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    //Check if it is true
    if((Buffer[byteIndex] & bitIndexer) > 0){
        return true;
    }

    //Othersise Return False
    return false;
}

//Assign flag to bitmap at index
bool Bitmap::Set(uint64_t index, bool value){
    //Validate Index
    if(index > Size * 8) return false;

    //Get the index of the byte
    uint64_t byteIndex = index / 8;

    //Get the index of the Bit
    uint8_t bitIndex = index % 8;

    //Calculate the Mask (Bitwise And)
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    //Set to false if it is true
    Buffer[byteIndex] &= ~bitIndexer;

    //If it is true
    if(value){
        //Set True
        Buffer[byteIndex] |= bitIndexer;
    }

    //Return It worked
    return true;
}