#pragma once

#include <stdint.h>
#include <stddef.h>

//Conversions
const char* to_string(uint64_t value);
const char* to_string(int64_t value);

//Hex Converter
const char* to_hstring(uint64_t value);
const char* to_hstring(uint32_t value);
const char* to_hstring(uint16_t value);
const char* to_hstring(uint8_t value);

//Decimal Playce Converter
const char* to_string(double value, uint8_t dp);
const char* to_string(double value);

//Compare 2 Strings
bool strcmp(const char* val1, const char* val2, size_t size);