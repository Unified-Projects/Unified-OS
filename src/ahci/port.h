#pragma once

#include <stdint.h>
#include "../IO/pci/pci.h"

//Drive definitions for modes
#define ATA_DEV_BUSY   0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_DEV_DRQ    0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EX       0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EX       0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EX      0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EX    0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define HBA_PxIS_TFES (1 << 30)

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

namespace AHCI{
    //Types
    enum PortType {
        None = 0,
        SATA = 1,
        SEMB = 2,
        PM = 3,
        SATAPI = 4,
    };

    enum FIS_TYPE{
        FIS_TYPE_REG_H2D = 0x27,
        FIS_TYPE_REG_D2H = 0x34,
        FIS_TYPE_DMA_ACT = 0x39,
        FIS_TYPE_DMA_SETUP = 0x41,
        FIS_TYPE_DATA = 0x46,
        FIS_TYPE_BIST = 0x58,
        FIS_TYPE_PIO_SETUP = 0x5F,
        FIS_TYPE_DEV_BITS = 0xA1,
    };
    
    //Porting
    struct HBAPort{
        uint32_t commandListBase;
        uint32_t commandListBaseUpper;
        uint32_t fisBaseAddress;
        uint32_t fisBaseAddressUpper;
        uint32_t interruptStatus;
        uint32_t interruptEnable;
        uint32_t cmdSts;
        uint32_t rsv0;
        uint32_t taskFileData;
        uint32_t signature;
        uint32_t sataStatus;
        uint32_t sataControl;
        uint32_t sataError;
        uint32_t sataActive;
        uint32_t commandIssue;
        uint32_t sataNotification;
        uint32_t fisSwitchControl;
        uint32_t rsv1[11];
        uint32_t vendor[4];
    };

    //Memory
    struct HBAMemory{
        uint32_t hostCapability;
        uint32_t globalHostControl;
        uint32_t interruptStatus;
        uint32_t portsImplemented;
        uint32_t version;
        uint32_t cccControl;
        uint32_t cccPorts;
        uint32_t enclosureManagementLocation;
        uint32_t enclosureManagementControl;
        uint32_t hostCapabilitiesExtended;
        uint32_t biosHandoffCtrlSts;
        uint8_t rsv0[0x74];
        uint8_t vendor[0x60];
        HBAPort ports[1];
    };
    
    //Commands
    struct HBACommandHeader {
        uint8_t commandFISLength:5;
        uint8_t atapi:1;
        uint8_t write:1;
        uint8_t prefetchable:1;

        uint8_t reset:1;
        uint8_t bist:1;
        uint8_t clearBusy:1;
        uint8_t rsv0:1;
        uint8_t portMultiplier:4;

        uint16_t prdtLength;
        uint32_t prdbCount;
        uint32_t commandTableBaseAddress;
        uint32_t commandTableBaseAddressUpper;
        uint32_t rsv1[4];
    };

    //Entry
    struct HBAPRDTEntry{
        uint32_t dataBaseAddress;
        uint32_t dataBaseAddressUpper;
        uint32_t rsv0;

        uint32_t byteCount:22;
        uint32_t rsv1:9;
        uint32_t interruptOnCompletion:1;
    };

    //Main HBA
    struct HBACommandTable{
        uint8_t commandFIS[64];

        uint8_t atapiCommand[16];

        uint8_t rsv[48];

        HBAPRDTEntry prdtEntry[];
    };

    struct FIS_REG_H2D {
        uint8_t fisType;

        uint8_t portMultiplier:4;
        uint8_t rsv0:3;
        uint8_t commandControl:1;

        uint8_t command;
        uint8_t featureLow;

        uint8_t lba0;
        uint8_t lba1;
        uint8_t lba2;
        uint8_t deviceRegister;

        uint8_t lba3;
        uint8_t lba4;
        uint8_t lba5;
        uint8_t featureHigh;

        uint8_t countLow;
        uint8_t countHigh;
        uint8_t isoCommandCompletion;
        uint8_t control;

        uint8_t rsv1[4];
    };

    //A Port
    class Port {
        public:
            //Info
            HBAPort* hbaPort;
            PortType portType;
            uint8_t* buffer;
            uint8_t portNumber;

            //Control
            void Configure();
            void StartCMD();
            void StopCMD();

            //Utilise
            bool Read(uint64_t sector, uint32_t sectorCount, void* buffer);
            void Write(uint64_t sector, uint16_t offset, uint64_t size, void* buffer); //Not Yet
    };
}