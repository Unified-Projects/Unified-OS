#pragma once

#include <stdint.h>
#include "../screen/rendering/BasicRenderer.h"
#include "../utils/cstr.h"
#include "../memory/efiMemory.h"
#include "../memory/memory.h"
#include "../Paging/Bitmap.h"
#include "../Paging/PageFrameAllocator.h"
#include "../Paging/PageMapIndexer.h"
#include "../Paging/Paging.h"
#include "../Paging/PageTableManager.h"
#include "../userinput/mouse.h"
#include "../IO/acpi/acpi.h"
#include "../IO/pci/pci.h"

//What is recieved from the bootloader
struct BootInfo{
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t DescriptorSize;
	ACPI::RSDP2* rsdp;
};

//Memory
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo{
    PageTableManager* pageTableManager;
};

//Intitialiser
KernelInfo InitializeKernel(BootInfo* bootInfo);
