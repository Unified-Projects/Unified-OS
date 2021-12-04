#ifndef __UNIFIED_OS_FS_GPT_H
#define __UNIFIED_OS_FS_GPT_H

#include <common/stdint.h>
#include <fs/ByteInteraction.h>

namespace UnifiedOS{
    namespace FileSystem{
        namespace GPT{
            #define GPT_HEADER_SIGNATURE_BIG_ENDIAN 0x5452415020494645
            #define GPT_HEADER_SIGNATURE_LITTLE_ENDIAN 0x4546492050415254

            //Header of the GPT partition
            struct GPTHeader{
                uint64_t Signature;
                uint32_t Revision;
                uint32_t Size;
                uint32_t Crc32;
                uint32_t Reserved;
                uint64_t CurrentLBA;
                uint64_t BackupLBA;
                uint64_t FirstLBA;
                uint64_t LastLBA;
                uint8_t DiskGUID[16];
                uint64_t PartitionTableLBA;
                uint32_t PartNum;
                uint32_t PartEntrySize;
                uint32_t PartEntriesCRC;
            } __attribute__((packed));

            //Entries in the GPT
            struct GPTPartitonEntry{
                uint8_t TypeGUID[16];
                uint8_t PartitionGUID[16];
                uint64_t StartLBA;
                uint64_t EndLBA;
                uint64_t Flags;
                uint8_t Name[72];
            } __attribute__((packed));

            //Types
            enum GPT_Types{
                //Every
                UNSUSED_ENTRY = 0, //00000000-0000-0000-0000-000000000000
                MBR_PARTITION_SCHEME = 1, //024DEE41-33E7-11D3-9D69-0008C781F39F
                EFI_SYSTEM_PARTITION = 2, //C12A7328-F81F-11D2-BA4B-00A0C93EC93B
                BIOS_BOOT_PARITITON = 3, //NA //21686148-6449-6E6F-744E-656564454649
                INTEL_FAST_FLASH_PARTITION = 4, //NA //D3BFE2DE-3DAF-11DF-BA40-E3A556D89593

                //Microsoft
                MICROSOFT_RESERVED_PARITITON = 5, //NA //E3C9E316-0B5C-4DB8-817D-F92DF00215AE
                BASIC_DATA_PARITITON = 6, //FAT/NTFS //EBD0A0A2-B9E5-4433-87C0-68B6B72699C7
                STORAGE_SPACES_PARTITION = 7, //NA //E75CAF8F-F680-4CEE-AFA3-B001E56EFC2D

                //Linux
                LINUX_FILESYSTEM_DATA = 8, //EXT //0FC63DAF-8483-4772-8E79-3D69D8477DE4
                LINUX_ROOT = 9, //EXT //4F68BCE3-E8CD-4DB1-96E7-FBCAF984B709
                LINUX_BOOT = 10, //EXT //BC13C2FF-59E6-4262-A352-B275FD6F7172
                LINUX_SWAP = 11, //EXT //0657FD6D-A4AB-43C4-84E5-0933C84B4F4F
                LINUX_HOME = 12, //EXT //933AC7E1-2EB4-4F13-B844-0E14E2AEF915

                //MACOS
                APFS_CONTAINER = 13, //APFS //7C3457EF-0000-11AA-AA11-00306543ECAC
                APFS_RECOVERY = 14, //APFS //52637672-7900-11AA-AA11-00306543ECAC
            };

            struct GUID{
                uint32_t Entry1;
                uint16_t Entry2;
                uint16_t Entry3;
                uint16_t Entry4;
                uint16_t Entry5H;
                uint32_t Entry5L;

                bool operator ==(GUID r){
                    if(this->Entry1 != r.Entry1){
                        return false;
                    }
                    if(this->Entry2 != r.Entry2){
                        return false;
                    }
                    if(this->Entry3 != r.Entry3){
                        return false;
                    }
                    if(this->Entry4 != r.Entry4){
                        return false;
                    }
                    if(this->Entry5H != r.Entry5H){
                        return false;
                    }
                    if(this->Entry5L != r.Entry5L){
                        return false;
                    }  

                    return true;
                }
                bool operator ==(GPT_Types r){
                    if(this->GetGUIDType() == r){
                        return true;
                    }
                    return false;
                }

                void operator =(uint8_t* GUID){
                    this->Entry1 = ((uint32_t*)GUID)[0];
                    this->Entry2 = ((uint16_t*)GUID)[2];
                    this->Entry3 = ((uint16_t*)GUID)[3];
                    this->Entry4 = Endian::BigEndian(((uint16_t*)GUID)[4]);
                    this->Entry5H = Endian::BigEndian(((uint16_t*)GUID)[5]);
                    this->Entry5L = Endian::BigEndian(((uint32_t*)GUID)[3]);
                }

                GPT_Types GetGUIDType(){
                    GPT_Types type;

                    //Check types
                    if(*this == GUID{0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00000000}){
                        type = UNSUSED_ENTRY;
                    }
                    else if(*this == GUID{0x024DEE41, 0x33E7, 0x11D3, 0x9D69, 0x0008, 0xC781F39F}){
                        type = MBR_PARTITION_SCHEME;
                    }
                    else if(*this == GUID{0xC12A7328, 0xF81F, 0x11D2, 0xBA4B, 0x00A0, 0xC93EC93B}){
                        type = EFI_SYSTEM_PARTITION;
                    }
                    else if(*this == GUID{0x21686148, 0x6449, 0x6E6F, 0x744E, 0x6565, 0x64454649}){
                        type = BIOS_BOOT_PARITITON;
                    }
                    else if(*this == GUID{0xD3BFE2DE, 0x3DAF, 0x11DF, 0xBA40, 0xE3A5, 0x56D89593}){
                        type = INTEL_FAST_FLASH_PARTITION;
                    }
                    else if(*this == GUID{0xE3C9E316, 0x0B5C, 0x4DB8, 0x817D, 0xF92D, 0xF00215AE}){
                        type = MICROSOFT_RESERVED_PARITITON;
                    }
                    else if(*this == GUID{0xEBD0A0A2, 0xB9E5, 0x4433, 0x87C0, 0x68B6, 0xB72699C7}){
                        type = BASIC_DATA_PARITITON;
                    }
                    else if(*this == GUID{0xE75CAF8F, 0xF680, 0x4CEE, 0xAFA3, 0xB001, 0xE56EFC2D}){
                        type = STORAGE_SPACES_PARTITION;
                    }
                    else if(*this == GUID{0x0FC63DAF, 0x8483, 0x4772, 0x8E79, 0x3D69, 0xD8477DE4}){
                        type = LINUX_FILESYSTEM_DATA;
                    }
                    else if(*this == GUID{0x4F68BCE3, 0xE8CD, 0x4DB1, 0x96E7, 0xFBCA, 0xF984B709}){
                        type = LINUX_BOOT;
                    }
                    else if(*this == GUID{0xBC13C2FF, 0x59E6, 0x4262, 0xA352, 0xB275, 0xFD6F7172}){
                        type = LINUX_ROOT;
                    }
                    else if(*this == GUID{0x0657FD6D, 0xA4AB, 0x43C4, 0x84E5, 0x0933, 0xC84B4F4F}){
                        type = LINUX_SWAP;
                    }
                    else if(*this == GUID{0x933AC7E1, 0x2EB4, 0x4F13, 0xB844, 0x0E14, 0xE2AEF915}){
                        type = LINUX_HOME;
                    }
                    else if(*this == GUID{0x7C3457EF, 0x0000, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC}){
                        type = APFS_CONTAINER;
                    }
                    else if(*this == GUID{0x52637672, 0x7900, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC}){
                        type = APFS_RECOVERY;
                    }
                    else{
                        type = UNSUSED_ENTRY;
                    }

                    return type;
                }
                void SetGUID(GPT_Types type){
                    //Check types
                    if(type == UNSUSED_ENTRY){
                        *this = GUID{0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00000000};
                    }
                    else if(type == MBR_PARTITION_SCHEME){
                        *this = GUID{0x024DEE41, 0x33E7, 0x11D3, 0x9D69, 0x0008, 0xC781F39F};
                    }
                    else if(type == EFI_SYSTEM_PARTITION){
                        *this = GUID{0xC12A7328, 0xF81F, 0x11D2, 0xBA4B, 0x00A0, 0xC93EC93B};
                    }
                    else if(type == BIOS_BOOT_PARITITON){
                        *this = GUID{0x21686148, 0x6449, 0x6E6F, 0x744E, 0x6565, 0x64454649};
                    }
                    else if(type == INTEL_FAST_FLASH_PARTITION){
                        *this = GUID{0xD3BFE2DE, 0x3DAF, 0x11DF, 0xBA40, 0xE3A5, 0x56D89593};
                    }
                    else if(type == MICROSOFT_RESERVED_PARITITON){
                        *this = GUID{0xE3C9E316, 0x0B5C, 0x4DB8, 0x817D, 0xF92D, 0xF00215AE};
                    }
                    else if(type == BASIC_DATA_PARITITON){
                        *this = GUID{0xEBD0A0A2, 0xB9E5, 0x4433, 0x87C0, 0x68B6, 0xB72699C7};
                    }
                    else if(type == STORAGE_SPACES_PARTITION){
                        *this = GUID{0xE75CAF8F, 0xF680, 0x4CEE, 0xAFA3, 0xB001, 0xE56EFC2D};
                    }
                    else if(type == LINUX_FILESYSTEM_DATA){
                        *this = GUID{0x0FC63DAF, 0x8483, 0x4772, 0x8E79, 0x3D69, 0xD8477DE4};
                    }
                    else if(type == LINUX_BOOT){
                        *this = GUID{0x4F68BCE3, 0xE8CD, 0x4DB1, 0x96E7, 0xFBCA, 0xF984B709};
                    }
                    else if(type == LINUX_ROOT){
                        *this = GUID{0xBC13C2FF, 0x59E6, 0x4262, 0xA352, 0xB275, 0xFD6F7172};
                    }
                    else if(type == LINUX_SWAP){
                        *this = GUID{0x0657FD6D, 0xA4AB, 0x43C4, 0x84E5, 0x0933, 0xC84B4F4F};
                    }
                    else if(type == LINUX_HOME){
                        *this = GUID{0x933AC7E1, 0x2EB4, 0x4F13, 0xB844, 0x0E14, 0xE2AEF915};
                    }
                    else if(type == APFS_CONTAINER){
                        *this = GUID{0x7C3457EF, 0x0000, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC};
                    }
                    else if(type == APFS_RECOVERY){
                        *this = GUID{0x52637672, 0x7900, 0x11AA, 0xAA11, 0x0030, 0x6543ECAC};
                    }
                    else{
                        *this = GUID{0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00000000};
                    }
                }
            };
        }
    }
}

#endif