#include "cstr.h"

#include "../screen/rendering/BasicRenderer.h"

char uintTo_StringOutput[128];
const char* to_string(uint64_t value){
    uint8_t size;
    uint64_t sizeTest = value;
    while(sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_StringOutput[size - index] = remainder + '0';
    uintTo_StringOutput[size + 1] = 0;
    return uintTo_StringOutput;
}

char intTo_StringOutput[128];
const char* to_string(int64_t value){
    uint8_t isNegative = 0;

    if(value < 0){
        isNegative = 1;
        value *= -1;
        intTo_StringOutput[0] = '-';
    }

    uint8_t size;
    uint64_t sizeTest = value;
    while(sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        intTo_StringOutput[isNegative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intTo_StringOutput[isNegative + size - index] = remainder + '0';
    intTo_StringOutput[isNegative + size + 1] = 0;
    return intTo_StringOutput;
}

char hexTo_StringOutput64[128];
const char* to_hstring(uint64_t value){
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;

    for(uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput64[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');

        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput64[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput64[size + 1] = 0;
    return hexTo_StringOutput64;
}

char hexTo_StringOutput32[128];
const char* to_hstring(uint32_t value){
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;

    for(uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');

        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput32[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput32[size + 1] = 0;
    return hexTo_StringOutput32;
}

char hexTo_StringOutput16[128];
const char* to_hstring(uint16_t value){
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;

    for(uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');

        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput16[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput16[size + 1] = 0;
    return hexTo_StringOutput16;
}

char hexTo_StringOutput8[128];
const char* to_hstring(uint8_t value){
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;

    for(uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');

        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput8[size - (i * 2 + 0)] = tmp + (tmp > 9 ? 55 : '0');
    }

    hexTo_StringOutput8[size + 1] = 0;
    return hexTo_StringOutput8;
}

char doubleTo_StringOutput[128];
const char* to_string(double value, uint8_t dp){
    if(dp > 20){
        dp = 20;
    }

    char* intPtr = (char*)to_string((int64_t)value);
    char* doublePtr = (char*)doubleTo_StringOutput;

    if(value < 0){
        value *= -1;
    }

    while(*intPtr != 0){
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for (uint8_t i = 0; i < dp; i++){
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleTo_StringOutput;
}

const char* to_string(double value){
    return to_string(value, 2);
}

bool strcmp(const char* val1, const char* val2, size_t size){
    for(int i = 0; i < size; i++){
        if(val1[i] != val2[i]){
            return false;
        }
    }
    
    return true;
}

int strlen(const char *str){
    int n = 0;
    for(const char* p = str; *p != '\0'; ++p, ++n);
    return n + 1;
}