#pragma once

#include <stdint.h>
#include <stddef.h>
#include "efiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t DescriptorSize);

void memset(void* start, uint8_t value, uint64_t num);

void* memcpy(void *dst, const void *src, size_t n);