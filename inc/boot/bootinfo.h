#ifndef __UNIFIED_OS_BOOT_BOOTINFO_H
#define __UNIFIED_OS_BOOT_BOOTINFO_H

#include <common/stdint.h>

namespace UnifiedOS{
	namespace Boot{
		//Font Header
		typedef struct {
			unsigned char magic[2];
			unsigned char mode;
			unsigned char charsize;
		} PSF1_HEADER;

		//Font
		typedef struct {
			PSF1_HEADER* psf1_Header;
			void* glyphBuffer;
		} PSF1_FONT;

		//IO
		namespace ACPI{
			struct RSDP2{
				unsigned char Signature[8];
				uint8_t Checksum;
				uint8_t OEMId[6];
				uint8_t Revision;
				uint32_t RSFTAddress;
				uint32_t Length;
				uint64_t XSDTAddress;
				uint8_t ExtendedChecksum;
				uint8_t Reseved[3];
			} __attribute__((packed));
		}

		//Memory
		struct EFI_MEMORY_DESCRIPTOR{
			uint32_t type;
			void* physAddr;
			void* virtAddr;
			uint64_t numPages;
			uint64_t attribs;
		};

		//Screen
		struct Framebuffer{
			void* BaseAddress;
			size_t BufferSize;
			unsigned int Width;
			unsigned int Height;
			unsigned int PixelsPerScanLine;
		};

		struct BootInfo{
			Framebuffer* framebuffer;
			PSF1_FONT* psf1_font;
			EFI_MEMORY_DESCRIPTOR* mMap;
			uint64_t mMapSize;
			uint64_t DescriptorSize;
			ACPI::RSDP2* rsdp; //ACPI
		};

		//Global
		extern BootInfo* __BOOT__BootContext__;
	}
}

#endif