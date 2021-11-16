#ifndef __UNIFIED_OS_IO_DEVICE_MANAGAER_H
#define __UNIFIED_OS_IO_DEVICE_MANAGAER_H

#include <IO/DeviceManager/ACPI/ACPI.h>
#include <IO/DeviceManager/PCI/pci.h>

#include <drivers/Driver.h>

namespace UnifiedOS{
    namespace Devices{
        class DeviceManager{
        protected:
            //Setups
            void LocateACPI();
            void SetupPCI(Drivers::DriverManager* DriverLoader);

            //ACPI
            ACPI::SDTHeader* xsdt;
            ACPI::MCFGHeader* mcfg;

            //PCI
            PCI* pci;
        public:
            DeviceManager(Drivers::DriverManager* DriverLoader);
        };
    }
}

#endif
