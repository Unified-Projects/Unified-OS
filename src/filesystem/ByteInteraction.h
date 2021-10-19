#pragma once

#include <stdint.h>

//Definitions
uint8_t GetByte(void* Buffer, uint64_t start);
uint16_t Get2Byte(void* Buffer, uint64_t start);
uint32_t Get4Byte(void* Buffer, uint64_t start);
uint64_t Get8Byte(void* Buffer, uint64_t start);

uint16_t Flip2Byte(uint16_t toFlip);
uint32_t Flip4Byte(uint32_t toFlip);
uint64_t Flip8Byte(uint64_t toFlip);