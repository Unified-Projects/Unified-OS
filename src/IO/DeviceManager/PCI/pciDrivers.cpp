#include <IO/DeviceManager/PCI/pci.h>
#include <paging/PageTableManager.h>

#include <common/stdio.h>

#include <drivers/Intel/AHCI/AHCI.h>

#include <memory/heap.h>

using namespace UnifiedOS;

using namespace UnifiedOS::Devices;
using namespace UnifiedOS::Devices::ACPI;

using namespace UnifiedOS::Paging;

//POTENTIALLY USELESS
uint8_t PCI::EnableDriverAtVendorName(PCIDeviceHeader* pciHeader){}
uint8_t PCI::EnableDriverAtDeviceName(PCIDeviceHeader* pciHeader){}
uint8_t PCI::EnableDriverAtSubclassName(PCIDeviceHeader* pciHeader){}

//Driver loading
uint8_t PCI::EnableDriverAtProgIFName(PCIDeviceHeader* pciHeader){
    switch (pciHeader->Class){
        case 0x01:
            switch (pciHeader->SubClass){
                case 0x06:
                    switch (pciHeader->ProgIF){
                        case 0x00:
                            // return "Vendor Specific Interface";
                            return 0x01;
                        case 0x01:{
                            // return "AHCI 1.0";
                            Drivers::Driver* AHCI = new Drivers::AHCI::AHCIDriver(pciHeader);
                            DriverM->AddDriver(AHCI);
                            return 0x01;
                            }
                        case 0x02:
                            // return "Serial Storage Bus";
                            return 0x01;
                    }
            }
        case 0x03:
            switch (pciHeader->SubClass){
                case 0x00:
                    switch (pciHeader->ProgIF){
                        case 0x00:
                            // return "VGA Controller";
                            return 0x01;
                        case 0x01:
                            // return "8514-Compatible Controller";
                            return 0x01;
                    }
            }
        case 0x0C:
            switch (pciHeader->SubClass){
                case 0x03:
                    switch (pciHeader->ProgIF){
                        case 0x00:
                            // return "UHCI Controller";
                            return 0x01;
                        case 0x10:
                            // return "OHCI Controller";
                            return 0x01;
                        case 0x20:
                            // return "EHCI (USB2) Controller";
                            return 0x01;
                        case 0x30:
                            // return "XHCI (USB3) Controller";
                            return 0x01;
                        case 0x80:
                            // return "Unspecified";
                            return 0x01;
                        case 0xFE:
                            // return "USB Device (Not a Host Controller)";
                            return 0x01;
                    }
            }    
    }
    return 0x00;
}