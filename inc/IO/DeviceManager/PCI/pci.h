#ifndef __UNIFIED_OS_IO_DEVICE_PCI_H
#define __UNIFIED_OS_IO_DEVICE_PCI_H

#include <IO/DeviceManager/ACPI/ACPI.h>

#include <drivers/Driver.h>

namespace UnifiedOS{
    namespace Devices{
        //PCI Device Info
        struct PCIDeviceHeader{
            uint16_t VendorID;
            uint16_t DeviceID;
            uint16_t Command;
            uint16_t Status;
            uint8_t RevisionID;
            uint8_t ProgIF;
            uint8_t SubClass;
            uint8_t Class;
            uint8_t CacheLineSize;
            uint8_t LatencyTumer;
            uint8_t HeaderType;
            uint8_t BIST;
        };

        //PCI Header Info
        struct PCIHeader0 {
            PCIDeviceHeader Header;
            uint32_t BAR0;
            uint32_t BAR1;
            uint32_t BAR2;
            uint32_t BAR3;
            uint32_t BAR4;
            uint32_t BAR5;
            uint32_t CardbusCISPtr;
            uint16_t SubsystemVendorID;
            uint16_t SubsystemID;
            uint32_t ExpansionROMBaseAddr;
            uint8_t CapabilitiesPtr;
            uint8_t Rsv0;
            uint16_t Rsv1;
            uint32_t Rsv2;
            uint8_t InterruptLine;
            uint8_t InterruptPin;
            uint8_t MinGrant;
            uint8_t MaxLatency;
        };

        class PCI{
        protected:
            //This will toggle drivers
            uint8_t EnableDriverAtVendorName(PCIDeviceHeader* pciHeader);
            uint8_t EnableDriverAtDeviceName(PCIDeviceHeader* pciHeader);
            uint8_t EnableDriverAtSubclassName(PCIDeviceHeader* pciHeader);
            uint8_t EnableDriverAtProgIFName(PCIDeviceHeader* pciHeader);

            //Reading
            void EnumerateFunction(uint64_t deviceAddress, uint64_t function);
            void EnumerateDevice(uint64_t busAddress, uint64_t device);
            void EnumerateBus(uint64_t baseAddress, uint64_t bus);

            //For other functions
            Drivers::DriverManager* DriverM;
        public:
            //Setup
            PCI(ACPI::MCFGHeader* MCFG, Drivers::DriverManager* DriverLoader);

            //Get the name of a device
            const char* GetVendorName(uint16_t vendorID);
            const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID);
            const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode);
            const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF);

            //Other Names
            const char* MassStorageControllerSubclassName(uint8_t subclassCode);
            const char* SerialBusControllerSubclassName(uint8_t subclassCode);
            const char* BridgeDeviceSubclassName(uint8_t subclassCode);
        };

        //Names
        extern const char* PCIDeviceClasses[];
    }
}

#endif