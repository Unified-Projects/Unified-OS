#include <drivers/Intel/AHCI/AHCI.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Devices;
using namespace UnifiedOS::Drivers;
using namespace UnifiedOS::Drivers::AHCI;
using namespace UnifiedOS::FileSystem;

#include <paging/PageFrameAllocator.h>
using namespace UnifiedOS::Paging;

#include <memory/memory.h>
using namespace UnifiedOS::Memory;

void AHCIPort::Configure(){
    //Stop ongoing processes
    StopCMD();

    //Data base setup
    void* newBase = __PAGING__PFA_GLOBAL.RequestPage();
    registers->commandListBase = (uint32_t)(uint64_t)newBase;
    registers->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
    memset((void*)(registers->commandListBase), 0, 1024);

    //Data fis setup
    void* fisBase = __PAGING__PFA_GLOBAL.RequestPage();
    registers->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
    registers->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
    memset(fisBase, 0, 256);

    //Processing
    HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)registers->commandListBase + ((uint64_t)registers->commandListBaseUpper << 32));

    //Size is 32 Bits
    for (int i = 0; i < 32; i++){
        cmdHeader[i].prdtLength = 8;

        //Get data
        void* cmdTableAddress = __PAGING__PFA_GLOBAL.RequestPage();
        uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
        cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)address;
        cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
        //Clear Thabe
        memset(cmdTableAddress, 0, 256);
    }
    
    //Restart process
    StartCMD();
}

void AHCIPort::StartCMD(){
    //Wait for proecess
    while (registers->cmdSts & HBA_PxCMD_CR);

    //Set Process
    registers->cmdSts |= HBA_PxCMD_FRE;
    registers->cmdSts |= HBA_PxCMD_ST;
}
void AHCIPort::StopCMD(){
    //Remvoe Process
    registers->cmdSts &= ~HBA_PxCMD_ST;
    registers->cmdSts &= ~HBA_PxCMD_FRE;

    //Wait for all to finish
    while(true){
        if (registers->cmdSts & HBA_PxCMD_FR) continue;
        if (registers->cmdSts & HBA_PxCMD_CR) continue;

        break;
    }
}
#include <common/stdio.h>
#include <common/cstring.h>
//Read/Write
int AHCIPort::ReadDiskBlock(uint64_t lba, size_t count, void* buffer){
    //Split the sector address
    uint32_t sectorL = (uint32_t) lba;
    uint32_t sectorH = (uint32_t) (lba >> 32);

    registers->interruptStatus = (uint32_t)-1; // Clear pending interrupt bits

    HBACommandHeader* cmdHeader = (HBACommandHeader*)registers->commandListBase;
    cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
    cmdHeader->write = 0; //this is a read
    cmdHeader->prdtLength = 1;

    //Set HBA Table
    HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
    memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

    //Add entries based on buffer for addresses and sizing
    commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
    commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
    commandTable->prdtEntry[0].byteCount = (count<<9)-1; // 512 bytes per sector
    commandTable->prdtEntry[0].interruptOnCompletion = 1;
    
    //Prepare
    FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

    //Enable read
    cmdFIS->fisType = FIS_TYPE_REG_H2D;
    cmdFIS->commandControl = 1; // command
    cmdFIS->command = ATA_CMD_READ_DMA_EX;
    
    //Set Sector Positions
    cmdFIS->lba0 = (uint8_t)sectorL;
    cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
    cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
    cmdFIS->lba3 = (uint8_t)sectorH;
    cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
    cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

    cmdFIS->deviceRegister = 1<<6; //LBA mode

    //Sectors
    cmdFIS->countLow = count & 0xFF;
    cmdFIS->countHigh = (count >> 8) & 0xFF;

    uint64_t spin = 0;

    //While unable do nothing
    while ((registers->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
        spin ++;
    }
    //If not working return no data
    if (spin == 1000000) {
        return false;
    }

    //Start Read
    registers->commandIssue = 1;

    //Read until done
    while (true){

        if((registers->commandIssue == 0)) break;
        if(registers->interruptStatus & HBA_PxIS_TFES)
        {
            return false;
        }
    }

    return true;
}
int AHCIPort::WriteDiskBlock(uint64_t lba, size_t count, void* buffer){
    //Split the sector address
    uint32_t sectorL = (uint32_t) lba;
    uint32_t sectorH = (uint32_t) (lba >> 32);

    registers->interruptStatus = (uint32_t)-1; // Clear pending interrupt bits

    HBACommandHeader* cmdHeader = (HBACommandHeader*)registers->commandListBase;
    cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
    cmdHeader->write = 1; //this is a write mode
    cmdHeader->prdtLength = 1;

    //Set HBA Table
    HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
    memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

    //Add entries based on buffer for addresses and sizing
    commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
    commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
    commandTable->prdtEntry[0].byteCount = (count<<9)-1; // 512 bytes per sector
    commandTable->prdtEntry[0].interruptOnCompletion = 1;
    
    //Prepare
    FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

    //Enable write mode command
    cmdFIS->fisType = FIS_TYPE_REG_H2D;
    cmdFIS->commandControl = 1; // command
    cmdFIS->command = ATA_CMD_WRITE_DMA_EX;
    
    //Set Sector Positions
    cmdFIS->lba0 = (uint8_t)sectorL;
    cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
    cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
    cmdFIS->lba3 = (uint8_t)sectorH;
    cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
    cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

    cmdFIS->deviceRegister = 1<<6; //LBA mode

    //Sectors
    cmdFIS->countLow = count & 0xFF;
    cmdFIS->countHigh = (count >> 8) & 0xFF;

    uint64_t spin = 0;

    //While busy
    while ((registers->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
        spin ++;
    }
    //If busy for too long exit
    if (spin == 1000000) {
        return false;
    }

    //Start write
    registers->commandIssue = 1;

    //Wait for writing to finish
    while (true){

        if((registers->commandIssue == 0)) break;
        if(registers->interruptStatus & HBA_PxIS_TFES)
        {
            return false;
        }
    }

    return true;
}

//Constructor
AHCIPort::AHCIPort(int num, HBAPort* portStructure, PortType port){
    //Save the port structure
    registers = portStructure;

    //Command setup
    registers->cmdSts &= ~HBA_PxCMD_ST;
    registers->cmdSts &= ~HBA_PxCMD_FRE;

    //Set name for DiksDevice
    SetName("SATA Hard Disk");

    //Setup the port
    Configure();

    //Store port information
    portType = port;
    portNumber = num;
    status = Active;
}