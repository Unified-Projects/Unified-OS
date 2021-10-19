#pragma once
#include <stdint.h>
#include "../IO/pci/pci.h"
#include "./port.h"

#include "../filesystem/drive.h"

//AHCI Namespace
namespace AHCI{
    class AHCIDriver{
    private:
        //Device
        PCI::PCIDeviceHeader* PCIBaseAddress;
    public:
        //Setup
        AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress);
        ~AHCIDriver();

        //Required Items
        HBAMemory* ABAR;
        void ProbePorts();

        Port* Ports[32];
        uint8_t portCount;
    };
}