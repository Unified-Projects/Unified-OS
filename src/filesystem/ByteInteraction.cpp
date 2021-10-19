#include "ByteInteraction.h"

//Simple read from buffer
uint8_t GetByte(void* Buffer, uint64_t start){
    return ((uint8_t*)Buffer)[start];
}

//Simple read 2 Bytes from buffer
uint16_t Get2Byte(void* Buffer, uint64_t start){
    uint8_t B1 = GetByte(Buffer, start);
    uint8_t B2 = GetByte(Buffer, start+1);

    return ((uint16_t)B1 << 8) | ((uint16_t)B2);
}

//Simple read 4 Bytes from buffer
uint32_t Get4Byte(void* Buffer, uint64_t start){
    uint16_t U16_1 = Get2Byte(Buffer, start);
    uint16_t U16_2 = Get2Byte(Buffer, start+2);

    return ((uint32_t)U16_1 << 16) | ((uint32_t)U16_2);
}

//Reads 8 Bytes of the buffer
uint64_t Get8Byte(void* Buffer, uint64_t start){
    uint64_t U32_1 = Get4Byte(Buffer, start);
    uint64_t U32_2 = Get4Byte(Buffer, start + 0x04);

    U32_1 <<= 32;

    return (U32_1 & 0xFFFFFFFF00000000) | (U32_2 & 0x00000000FFFFFFFF);
}

//Simple when reading from disk
//With some numbers we want to swap the two bytes
uint16_t Flip2Byte(uint16_t toFlip){
    uint16_t B1 = (toFlip & 0x00FF) << 8;
    uint16_t B2 = (toFlip & 0xFF00) >> 8;
    
    return B1 | B2;
}

//Or Four
uint32_t Flip4Byte(uint32_t toFlip){
    uint32_t B1 = (toFlip & 0x000000FF) << 24;
    uint32_t B2 = (toFlip & 0x0000FF00) << 8;
    uint32_t B3 = (toFlip & 0x00FF0000) >> 8;
    uint32_t B4 = (toFlip & 0xFF000000) >> 24;

    return B1 | B2 | B3 | B4;
}

//Or Eight
uint64_t Flip8Byte(uint64_t toFlip){
    uint64_t B1 = (toFlip & 0x00000000000000FF) << 56;
    uint64_t B2 = (toFlip & 0x000000000000FF00) << 40;
    uint64_t B3 = (toFlip & 0x0000000000FF0000) << 24;
    uint64_t B4 = (toFlip & 0x00000000FF000000) << 8;
    uint64_t B5 = (toFlip & 0x000000FF00000000) >> 8;
    uint64_t B6 = (toFlip & 0x0000FF0000000000) >> 24;
    uint64_t B7 = (toFlip & 0x00FF000000000000) >> 40;
    uint64_t B8 = (toFlip & 0xFF00000000000000) >> 56;

    return B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8;
}