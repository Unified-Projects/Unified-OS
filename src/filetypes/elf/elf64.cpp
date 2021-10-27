#include "elf64.h"
#include "../../filesystem/ByteInteraction.h"

#include "../../screen/rendering/BasicRenderer.h"
#include "../../memory/memory.h"
#include "../../memory/heap.h"

ELF64_Header GetHeader(void* buffer){
    ELF64_Header Header = {};

    //Read Signature
    Header.Signature = Get4Byte(buffer, 0x00);

    //Validate
    if(Header.Signature != 0x7F454C46) return Header;

    //Read Bit (64Bit)
    Header.Bits = GetByte(buffer, 0x04);

    //Validate
    if(Header.Bits != 2) return Header;

    //Rest
    Header.Endian = GetByte(buffer, 0x05);
    Header.ELFHeaderVersion = GetByte(buffer, 0x06);
    Header.OSABI = GetByte(buffer, 0x07);
    //Padding Bytes 8-15
    switch (Header.Endian)
    {
    case 1:
        Header.Type = LittleEndian(Get2Byte(buffer, 16));
        Header.InstructionSet = LittleEndian(Get2Byte(buffer, 18));
        Header.ELFVersion = LittleEndian(Get4Byte(buffer, 20));
        Header.EntryPosition = LittleEndian(Get8Byte(buffer, 24));
        Header.HeaderTablePosition = LittleEndian(Get8Byte(buffer, 32));
        Header.SectionHeaderPosition = LittleEndian(Get8Byte(buffer, 40));
        Header.Flags = LittleEndian(Get4Byte(buffer, 48));
        Header.HeaderSize = LittleEndian(Get2Byte(buffer, 52));
        Header.SizeOfEInPHeader = LittleEndian(Get2Byte(buffer, 54));
        Header.NumberOfEInPHeader = LittleEndian(Get2Byte(buffer, 56));
        Header.SizeOfEInSHeader = LittleEndian(Get2Byte(buffer, 58));
        Header.NumberOfEInSHeader = LittleEndian(Get2Byte(buffer, 60));
        Header.IndexInSectionHeaderEntries = LittleEndian(Get2Byte(buffer, 62));
        break;

    case 2:
        Header.Type = Get2Byte(buffer, 16);
        Header.InstructionSet = Get2Byte(buffer, 18);
        Header.ELFVersion = Get4Byte(buffer, 20);
        Header.EntryPosition = Get8Byte(buffer, 24);
        Header.HeaderTablePosition = Get8Byte(buffer, 32);
        Header.SectionHeaderPosition = Get8Byte(buffer, 40);
        Header.Flags = Get4Byte(buffer, 48);
        Header.HeaderSize = Get2Byte(buffer, 52);
        Header.SizeOfEInPHeader = Get2Byte(buffer, 54);
        Header.NumberOfEInPHeader = Get2Byte(buffer, 56);
        Header.SizeOfEInSHeader = Get2Byte(buffer, 58);
        Header.NumberOfEInSHeader = Get2Byte(buffer, 60);
        Header.IndexInSectionHeaderEntries = Get2Byte(buffer, 62);
        break;
    }

    return Header;
}

ELF64_Program_Header GetPHeader(void* Buffer){
    ELF64_Program_Header ph = {};

    ph.Type = LittleEndian(Get4Byte(Buffer, 0));
    ph.Flags = LittleEndian(Get4Byte(Buffer, 4));
    ph.OffSetInData = LittleEndian(Get8Byte(Buffer, 8));
    ph.virtualAddrress = LittleEndian(Get8Byte(Buffer, 16));
    ph.SVABI = LittleEndian(Get8Byte(Buffer, 24));
    ph.SizeofSegment = LittleEndian(Get8Byte(Buffer, 32));
    ph.SizeOfMemory = LittleEndian(Get8Byte(Buffer, 40));
    ph.RequiredAllignment = LittleEndian(Get8Byte(Buffer, 48));

    return ph;
}

ELF64_Section_Header GetSHeader(void* Buffer){
    ELF64_Section_Header sh = {};

    sh.Name = LittleEndian(Get4Byte(Buffer, 0));
    sh.Type = LittleEndian(Get4Byte(Buffer, 4));
    sh.Flags = LittleEndian(Get8Byte(Buffer, 8));
    sh.Addr = LittleEndian(Get8Byte(Buffer, 16));
    sh.Offst = LittleEndian(Get8Byte(Buffer, 24));
    sh.Size = LittleEndian(Get8Byte(Buffer, 32));
    sh.Link = LittleEndian(Get4Byte(Buffer, 40));
    sh.Info = LittleEndian(Get4Byte(Buffer, 44));
    sh.AddrAlign = LittleEndian(Get8Byte(Buffer, 48));
    sh.EntrySize = LittleEndian(Get8Byte(Buffer, 56));

    return sh;
}

//Constructs
Executable::Executable(const char* path){
    File = GlobalFileManager.FindFile(path);
}
Executable::Executable(FILE::File file){
    File = file;
}
Executable::~Executable(){
    File.FreeData();
}

void Executable::LoadELF(){
    
}

void Executable::DebugHeader(){
    File.ReadData();
    Header = GetHeader(File.data);

    uint8_t* PBuffer = (uint8_t*)File.data;
    offsetBuffer(PBuffer, Header.HeaderTablePosition);

    uint8_t* SBuffer = (uint8_t*)File.data;
    offsetBuffer(SBuffer, Header.SectionHeaderPosition);

    PHeaders = new ELF64_Program_Header[Header.NumberOfEInPHeader];

    for(int i = 0; i < Header.NumberOfEInPHeader; i++){
        PHeaders[i] = GetPHeader((void*)PBuffer);

        offsetBuffer(PBuffer, Header.SizeOfEInPHeader);
    }

    uint64_t fSize = 0;

    SHeaders = new ELF64_Section_Header[Header.NumberOfEInSHeader];

    for(int i = 0; i < Header.NumberOfEInSHeader; i++){
        SHeaders[i] = GetSHeader((void*)SBuffer);

        if(SHeaders[i].Type == 0x2){
            uint8_t* SYMBuffer = (uint8_t*)File.data;
            offsetBuffer(SYMBuffer, SHeaders[i].Offst);

            Symbols = new ELF64_SYM[SHeaders[i].Size / SHeaders[i].EntrySize];

            for(int j = 0; j < SHeaders[i].Size / SHeaders[i].EntrySize; j++){

                Symbols[j] = {};

                Symbols[j].SymName = LittleEndian(Get4Byte(SYMBuffer, 0x00));
                offsetBuffer(SYMBuffer, 4);

                Symbols[j].Info = GetByte(SYMBuffer, 0x00);
                offsetBuffer(SYMBuffer, 1);

                Symbols[j].Other = GetByte(SYMBuffer, 0x00);
                offsetBuffer(SYMBuffer, 1);

                Symbols[j].SectionIndex = LittleEndian(Get2Byte(SYMBuffer, 0x00));
                offsetBuffer(SYMBuffer, 2);

                Symbols[j].Value = LittleEndian(Get8Byte(SYMBuffer, 0x00));
                offsetBuffer(SYMBuffer, 8);

                Symbols[j].Size = LittleEndian(Get8Byte(SYMBuffer, 0x00));
                offsetBuffer(SYMBuffer, 8);

                //offsetBuffer(SYMBuffer, sh.EntrySize);
            }

            SymbolCount = SHeaders[i].Size / SHeaders[i].EntrySize;
        }
        else if(SHeaders[i].Type == 3 & ! StringTableLoaded){
            StringTableLoaded = true;

            uint32_t CurrentSizePos = 1;

            while(CurrentSizePos < SHeaders[i].Size){
                uint32_t StringSize = 0;

                for(int j = CurrentSizePos; j < SHeaders[i].Size; j++){
                    if(GetByte(File.data, SHeaders[i].Offst + j) != 0x00)
                        StringSize++;
                    else
                        break;
                }

                StringCount++;

                CurrentSizePos += StringSize + 1;
            }

            CurrentSizePos = 1;

            Strings = new ELF_STR_ENTRY[StringCount];
            uint32_t StringIndex = 0;

            while(CurrentSizePos < SHeaders[i].Size){
                uint32_t StringSize = 0;
                
                for(int j = CurrentSizePos; j < SHeaders[i].Size; j++){
                    if(GetByte(File.data, SHeaders[i].Offst + j) != 0x00)
                        StringSize++;
                    else
                        break;
                }

                Strings[StringIndex] = {};
                Strings[StringIndex].String = new char[StringSize];
                Strings[StringIndex].Offset = CurrentSizePos;

                for(int j = 0; j < StringSize; j++){
                    Strings[StringIndex].String[j] = GetByte(File.data, SHeaders[i].Offst + j + CurrentSizePos);
                }

                StringIndex++;

                CurrentSizePos += StringSize + 1;
            }
        }
        else if(SHeaders[i].Type == 3 & StringTableLoaded){
            
        }

        offsetBuffer(SBuffer, Header.SizeOfEInSHeader);
    }
    
    //Load data
    Sections = new HSection[Header.NumberOfEInPHeader];
    for(uint64_t i = 0; i < Header.NumberOfEInPHeader; i++){
        ELF64_Program_Header PHeader = PHeaders[i];

        Sections[i] = {NULL, i + 1, PHeader.SizeOfMemory};

        if(PHeader.Type == PT_LOAD){
            void* DataEntry = (void*)((uint8_t*)File.data + PHeader.OffSetInData);
            Sections[i].Data = malloc(PHeader.SizeOfMemory);
            memcpy(Sections[i].Data, DataEntry, PHeader.SizeOfMemory);
        }
    }

    File.FreeData();
}

Function Executable::GetEntry(){
    for(uint32_t i = 0; i < SymbolCount; i++){
        ELF64_SYM Symbol = Symbols[i];
        uint64_t Offset = 0;

        if(Symbol.SymName != 0){
            if((Symbol.Info & 0x0F) == STT_FUNC){
                Offset += Symbol.Size;
                for(int j = 0; j < StringCount; j++){
                    if(Symbol.SymName == Strings[j].Offset){
                        return (int(*)())((uint8_t*)Sections[Symbol.SectionIndex - 1].Data + Offset);
                    }
                }
            }
        }
    }

    return NULL;
}