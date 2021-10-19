#include "ahci.h"
#include "../screen/rendering/BasicRenderer.h"
#include "../Paging/PageTableManager.h"
#include "../Paging/PageFrameAllocator.h"
#include "../memory/heap.h"
#include "../memory/memory.h"
#include "../utils/cstr.h"

namespace AHCI{

    //Drive defines
    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE 0x1
    #define SATA_SIG_ATAPI 0xEB140101
    #define SATA_SIG_ATA 0x00000101
    #define SATA_SIG_SEMB 0xC33C0101
    #define SATA_SIG_PM 0x96690101

    #define HBA_PxCMD_CR 0x8000
    #define HBA_PxCMD_FRE 0x0010
    #define HBA_PxCMD_ST 0x0001
    #define HBA_PxCMD_FR 0x4000

    //Port configuration
    void Port::Configure(){
        //Stop ongoing processes
        StopCMD();

        //Data base setup
        void* newBase = GlobalAllocator.RequestPage();
        hbaPort->commandListBase = (uint32_t)(uint64_t)newBase;
        hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
        memset((void*)(hbaPort->commandListBase), 0, 1024);

        //Data fis setup
        void* fisBase = GlobalAllocator.RequestPage();
        hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
        hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
        memset(fisBase, 0, 256);

        //Processing
        HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)hbaPort->commandListBase + ((uint64_t)hbaPort->commandListBaseUpper << 32));

        //Size is 32 Bits
        for (int i = 0; i < 32; i++){
            cmdHeader[i].prdtLength = 8;

            //Get data
            void* cmdTableAddress = GlobalAllocator.RequestPage();
            uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
            cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)address;
            cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
            //Clear Thabe
            memset(cmdTableAddress, 0, 256);
        }
        
        //Restart process
        StartCMD();
    }
    
    //Start CMD
    void Port::StartCMD(){
        //Wait for proecess
        while (hbaPort->cmdSts & HBA_PxCMD_CR);

        //Set Process
        hbaPort->cmdSts |= HBA_PxCMD_FRE;
        hbaPort->cmdSts |= HBA_PxCMD_ST;
    }

    //Stop Process
    void Port::StopCMD(){
        //Remvoe Process
        hbaPort->cmdSts &= ~HBA_PxCMD_ST;
        hbaPort->cmdSts &= ~HBA_PxCMD_FRE;

        //Wait for all to finish
        while(true){
            if (hbaPort->cmdSts & HBA_PxCMD_FR) continue;
            if (hbaPort->cmdSts & HBA_PxCMD_CR) continue;

            break;
        }
    }

    //Read from Port (Usually a drive)
    bool Port::Read(uint64_t sector, uint32_t sectorCount, void* buffer){
        //Split the sector address
        uint32_t sectorL = (uint32_t) sector;
        uint32_t sectorH = (uint32_t) (sector >> 32);

        hbaPort->interruptStatus = (uint32_t)-1; // Clear pending interrupt bits

        HBACommandHeader* cmdHeader = (HBACommandHeader*)hbaPort->commandListBase;
        cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->write = 0; //this is a read
        cmdHeader->prdtLength = 1;

        //Set HBA Table
        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

        //Add entries based on buffer for addresses and sizing
        commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->prdtEntry[0].byteCount = (sectorCount<<9)-1; // 512 bytes per sector
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
        cmdFIS->countLow = sectorCount & 0xFF;
        cmdFIS->countHigh = (sectorCount >> 8) & 0xFF;

        uint64_t spin = 0;

        //While unable do nothing
        while ((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
            spin ++;
        }
        //If not working return no data
        if (spin == 1000000) {
            return false;
        }

        //Start Read
        hbaPort->commandIssue = 1;

        //Read until done
        while (true){

            if((hbaPort->commandIssue == 0)) break;
            if(hbaPort->interruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
    }
    
    //Returns the type of a port
    PortType CheckPortType(HBAPort* port){
        //Staus
        uint32_t sataStatus = port->sataStatus;

        //Device Management
        uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
        uint8_t deviceDetection = sataStatus & 0b111;

        //If Not Present Return No Type
        if (deviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
        if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

        //Look for a type in the signature
        switch (port->signature){
            case SATA_SIG_ATAPI:
                return PortType::SATAPI;
            case SATA_SIG_ATA:
                return PortType::SATA;
            case SATA_SIG_PM:
                return PortType::PM;
            case SATA_SIG_SEMB:
                return PortType::SEMB;
            default:
                PortType::None;
        }
    }
    
    //Initiation of a driver
    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress){
        //PCI device Location
        this->PCIBaseAddress = pciBaseAddress;

        //BAR
        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

        //Mapping for BAR
        GlobalPTM.MapMemory(ABAR, ABAR);

        //Check For Ports
        ProbePorts();

        //Loop Over Ports
        for (int i = 0; i < portCount; i++){
            //Create a pointer
            Port* port = Ports[i];

            //Configure
            port->Configure();

            //If it is SATA then it is a drive and read from it
            if(port->portType == SATA){
                //Get a page to hold boot sector in its buffer
                port->buffer = (uint8_t*)GlobalAllocator.RequestPage();

                //Load Drive
                Drives[DriveCount++] = new Drive(port);
            }
        }
    }
    
    //No Driver Avalable
    AHCIDriver::~AHCIDriver(){

    }

    //Looks for ports in a device
    void AHCIDriver::ProbePorts(){
        //Reads from BAR
        uint32_t portsImplemented = ABAR->portsImplemented;
        //32 Max
        for(int i = 0; i < 32; i++){
            //If it exists
            if(portsImplemented & (1 << i)){
                //Get Type
                PortType portType = CheckPortType(&ABAR->ports[i]);
                
                //Sata ONLY create a access port
                if(portType == PortType::SATA || portType == PortType::SATAPI){
                    //Set intiaial variable
                    Ports[portCount] = new Port();
                    Ports[portCount]->portType = portType;
                    Ports[portCount]->hbaPort = &ABAR->ports[i];
                    Ports[portCount]->portNumber = portCount;
                    portCount++;
                }
            }
        }
    }
}