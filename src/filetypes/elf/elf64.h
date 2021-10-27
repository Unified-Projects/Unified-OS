#pragma once

#include <stdint.h>

#include "../../filesystem/fileManager.h"

typedef uint16_t Elf64_Half;	//Unsigned half int
typedef uint64_t Elf64_Off;	    //Unsigned offset
typedef uint64_t Elf64_Addr;	//Unsigned address
typedef uint32_t Elf64_Word;	//Unsigned int
typedef int32_t  Elf64_Sword;	//Signed int

struct ELF64_Header
{
    uint32_t Signature; //0x7F then ELF
    uint8_t Bits; //1 = 32Bit 2 = 64Bit
    uint8_t Endian; //1 = litte 2 = big
    uint8_t ELFHeaderVersion;
    uint8_t OSABI; //Usually 0
    //Padding Bytes 8-15
    uint16_t Type; //1 = relocatable, 2 = executable, 3 = shared, 4 = core
    uint16_t InstructionSet;
    uint32_t ELFVersion;
    uint64_t EntryPosition;
    uint64_t HeaderTablePosition;
    uint64_t SectionHeaderPosition;
    uint32_t Flags;
    uint16_t HeaderSize;
    uint16_t SizeOfEInPHeader;
    uint16_t NumberOfEInPHeader;
    uint16_t SizeOfEInSHeader;
    uint16_t NumberOfEInSHeader;
    uint16_t IndexInSectionHeaderEntries;
} __attribute__((Packed));

struct ELF64_Program_Header
{
    //Segment types: 
    //0 = null - ignore the entry; 
    //1 = load - clear p_memsz bytes at p_vaddr to 0, then copy p_filesz bytes from p_offset to p_vaddr; 
    //2 = dynamic - requires dynamic linking; 
    //3 = interp - contains a file path to an executable to use as an interpreter for the following segment; 
    //4 = note section. 
    //There are more values, but mostly contain architecture/environment specific information, which is probably not required for the majority of ELF files.
    
    //Flags: 
    //1 = executable
    //2 = writable
    //4 = readable
    uint32_t Type;
    uint32_t Flags;
    uint64_t OffSetInData;
    uint64_t virtualAddrress;
    uint64_t SVABI;
    uint64_t SizeofSegment;
    uint64_t SizeOfMemory;
    uint64_t RequiredAllignment; //Power of 2
} __attribute__((Packed));

struct ELF64_Section_Header
{   
    //Type:
    // Value	Name	Meaning
    // 0x0	SHT_NULL	Section header table entry unused
    // 0x1	SHT_PROGBITS	Program data
    // 0x2	SHT_SYMTAB	Symbol table
    // 0x3	SHT_STRTAB	String table
    // 0x4	SHT_RELA	Relocation entries with addends
    // 0x5	SHT_HASH	Symbol hash table
    // 0x6	SHT_DYNAMIC	Dynamic linking information
    // 0x7	SHT_NOTE	Notes
    // 0x8	SHT_NOBITS	Program space with no data (bss)
    // 0x9	SHT_REL	Relocation entries, no addends
    // 0x0A	SHT_SHLIB	Reserved
    // 0x0B	SHT_DYNSYM	Dynamic linker symbol table
    // 0x0E	SHT_INIT_ARRAY	Array of constructors
    // 0x0F	SHT_FINI_ARRAY	Array of destructors
    // 0x10	SHT_PREINIT_ARRAY	Array of pre-constructors
    // 0x11	SHT_GROUP	Section group
    // 0x12	SHT_SYMTAB_SHNDX	Extended section indices
    // 0x13	SHT_NUM	Number of defined types.
    // 0x60000000	SHT_LOOS	Start OS-specific.

    uint32_t Name;
    uint32_t Type;
    uint64_t Flags;
    uint64_t Addr;
    uint64_t Offst;
    uint64_t Size;
    uint32_t Link;
    uint32_t Info;
    uint64_t AddrAlign;
    uint64_t EntrySize;

} __attribute__((Packed));

struct ELF64_SYM
{
    uint32_t SymName;
    uint8_t Info;
    uint8_t Other; //Visibility
    uint64_t SectionIndex;
    uint64_t Value;
    uint64_t Size;
} __attribute__((Packed));

struct ELF64_Rel
{
    uint64_t offset;
    uint64_t info;
} __attribute__((Packed));

struct ELF64_Rela
{
    uint64_t offset;
    uint64_t info;
    uint64_t addend;
} __attribute__((Packed));

#define ELF64_R_SYM(i) ((i) >> 32) // Relocation info to symbol table index
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL) // Relocation info to relocation type
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL)) // Create relocation info value out of index s and type t

#define DT_NULL 0
#define DT_NEEDED 1
#define DT_PTRELSZ 2 // Size of the PLT relocaiton entries
#define DT_PLTGOT 3 // Address associated with the PLT
#define DT_HASH 4 // Symbol hashtable address
#define DT_STRTAB 5 // Dyanmic String Table
#define DT_SYMTAB 6 // Dynamic Symbol Table
#define DT_RELA 7 // Address of relocaiton table
#define DT_RELASZ 8 // Total size (bytes) of the relocation table
#define DT_RELAENT 9 // Total size (bytes) of each relocation entry
#define DT_STRSZ 10 // Total size (bytes) of string table
#define DT_SYMENT 11 // Size (bytes) of symbol table entry
#define DT_INIT 12 // Address of init function
#define DT_FINI 13 // Address of termination function
#define DT_SONAME 14 // String table object of the shared object name
#define DT_RPATH 15 // Table offset of a shared library search path string
#define DT_SYMBOLIC 16

#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19
#define DT_PLTREL 20
#define DT_DEBUG 21
#define DT_TEXTREL 22
#define DT_JMPREL 23
#define DT_BIND_NOW 24
#define DT_INIT_ARRAY 25
#define DT_FINI_ARRAY 26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6

// Section Types
#define SHT_NULL 0 // Unused
#define SHT_PROGBITS 1 // Information defined by the program
#define SHT_SYMTAB 2 // Symbol table
#define SHT_STRTAB 3 // String table
#define SHT_RELA 4 // 'Rela' relocation entires
#define SHT_HASH 5 // Hash table
#define SHT_DYNAMIC 6 // Dyanmic linking tables
#define SHT_NOTE 7 // Note
#define SHT_NOBITS 8 // Uninitialized 
#define SHT_REL 9 // 'Rel' relocation entries
#define SHT_DYNSYM 11 // Dynamic loader symbol table

// Section Attributes
#define SHF_WRITE 0x1 // Writable
#define SHF_ALLOC 0x2 // Allocated in memory for program
#define SHF_EXECINSTR 0x4

// .bss: SHT_NOBITS
// .data, .interp, .rodata, .text: SHT_PROGBITS

// Symbol Types
#define STT_NOTYPE 0 // No type specified
#define STT_OBJECT 1 // Data object
#define STT_FUNC 2 // Function entry point
#define STT_SECTION 3 // Section symbol
#define STT_FILE 4 // Source file associated with object file

// Symbol Bindings
#define STB_LOCAL 0 // Only visible within object file
#define STB_GLOBAL 1 // Global symbol
#define STB_WEAK 2 // Global scope, lower precedence than global symbols

#define ELF64_SYM_TYPE(s) ((s) & 0xf) // Symbol info value to symbol type only
#define ELF64_SYM_BIND(s) (((s) >> 4) & 0xf) // Symbol info value to symbol binding only

#define ELF64_R_SYM(i) ((i) >> 32) // Symbol table index from relocation info field
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL) // Relocation type from relocation info

// Relocation types
#define ELF64_R_X86_64_NONE 0
#define ELF64_R_X86_64_64 1 // symbol + addend
#define ELF64_R_X86_64_32 10 // symbol (32-bit) + addend
#define ELF64_R_X86_64_32S 11 // symbol (32-bit, sign extended) + addend

struct ELF64_Info{
	uint64_t entry;
	uint64_t pHdrSegment;
	uint64_t phEntrySize;
	uint64_t phNum;

	char* linkerPath;
};

struct HSection
{
    void* Data;
    uint32_t Index;
    uint64_t Size;
};

struct ELF_STR_ENTRY
{
    char* String;
    uint32_t Offset;
};


typedef int(*Function)();

class Executable{
private:
    FILE::File File;

    ELF64_Header Header;

    ELF64_Program_Header* PHeaders;
    ELF64_Section_Header* SHeaders;

    ELF_STR_ENTRY* Strings;
    uint32_t StringCount = 0;

    ELF_STR_ENTRY* Section_Header_Strings;
    uint32_t SectionStringsCount = 0;;

    bool StringTableLoaded = false;

    HSection* Sections;
    ELF64_SYM* Symbols;
    uint32_t SymbolCount;

    ELF64_Info ElfInfo;

    void* EntryBuffer;
    
public:
    Executable(const char* path);
    Executable(FILE::File file);
    ~Executable();

    void LoadELF();

    void DebugHeader();

    Function GetEntry();
};