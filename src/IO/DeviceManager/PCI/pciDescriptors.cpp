#include <IO/DeviceManager/PCI/pci.h>
#include <common/cstring.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Devices;

//Types of devices
//I dont know why i made this, i guess if you want the class name (to be implemented i guess)
const char* Devices::PCIDeviceClasses[] {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station", 
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerator",
    "Non Essential Instrumentation"
};

//(More to be added I think)
const char* Devices::PCI::GetVendorName(uint16_t vendorID){
    switch (vendorID){
        case 0x8086:
            return "Intel Corp";
        case 0x1022:
            return "AMD";
        case 0x10DE:
            return "NVIDIA Corporation";
    }
    return to_hstring(vendorID);
}
//(More to be added I think)
const char* Devices::PCI::GetDeviceName(uint16_t vendorID, uint16_t deviceID){
    switch (vendorID){
        case 0x8086: // Intel
            switch(deviceID){
                case 0x10D3:
                    return "82574L Gigabit Network Connection";
                case 0x29C0:
                    return "Express DRAM Controller";
                case 0x2918:
                    return "LPC Interface Controller";
                case 0x2922:
                    return "6 port SATA Controller [AHCI mode]";
                case 0x2930:
                    return "SMBus Controller";
                case 0x9C03:
                    return "8 Series SATA Controller 1 [AHCI mode]";
            }
    }
    return to_hstring(deviceID);
}
//(More to be added I think)
const char* Devices::PCI::MassStorageControllerSubclassName(uint8_t subclassCode){
    switch (subclassCode){
        case 0x00:
            return "SCSI Bus Controller";
        case 0x01:
            return "IDE Controller";
        case 0x02:
            return "Floppy Disk Controller";
        case 0x03:
            return "IPI Bus Controller";
        case 0x04:
            return "RAID Controller";
        case 0x05:
            return "ATA Controller";
        case 0x06:
            return "Serial ATA";
        case 0x07:
            return "Serial Attached SCSI";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Other";
    }
    return to_hstring(subclassCode);
}
//(More to be added I think)
const char* Devices::PCI::SerialBusControllerSubclassName(uint8_t subclassCode){
    switch (subclassCode){
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus";
        case 0x02:
            return "SSA";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel";
        case 0x05:
            return "SMBus";
        case 0x06:
            return "Infiniband";
        case 0x07:
            return "IPMI Interface";
        case 0x08:
            return "SERCOS Interface (IEC 61491)";
        case 0x09:
            return "CANbus";
        case 0x80:
            return "SerialBusController - Other";
    }
    return to_hstring(subclassCode);
}
//(More to be added I think)
const char* Devices::PCI::BridgeDeviceSubclassName(uint8_t subclassCode){
    switch (subclassCode){
        case 0x00:
            return "Host Bridge";
        case 0x01:
            return "ISA Bridge";
        case 0x02:
            return "EISA Bridge";
        case 0x03:
            return "MCA Bridge";
        case 0x04:
            return "PCI-to-PCI Bridge";
        case 0x05:
            return "PCMCIA Bridge";
        case 0x06:
            return "NuBus Bridge";
        case 0x07:
            return "CardBus Bridge";
        case 0x08:
            return "RACEway Bridge";
        case 0x09:
            return "PCI-to-PCI Bridge";
        case 0x0a:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Other";
    }
    return to_hstring(subclassCode);
}
//(More to be added I think)
const char* Devices::PCI::GetSubclassName(uint8_t classCode, uint8_t subclassCode){
    switch (classCode){
        case 0x01:
            return MassStorageControllerSubclassName(subclassCode);
        case 0x03:
            switch (subclassCode){
                case 0x00:
                    return "VGA Compatible Controller";
            }
        case 0x06:
            return BridgeDeviceSubclassName(subclassCode);
        case 0x0C:
            return SerialBusControllerSubclassName(subclassCode);
    }
    return to_hstring(subclassCode);
}
//(More to be added I think)
const char* Devices::PCI::GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF){
    switch (classCode){
        case 0x01:
            switch (subclassCode){
                case 0x06:
                    switch (progIF){
                        case 0x00:
                            return "Vendor Specific Interface";
                        case 0x01:
                            return "AHCI 1.0";
                        case 0x02:
                            return "Serial Storage Bus";
                    }
            }
        case 0x03:
            switch (subclassCode){
                case 0x00:
                    switch (progIF){
                        case 0x00:
                            return "VGA Controller";
                        case 0x01:
                            return "8514-Compatible Controller";
                    }
            }
        case 0x0C:
            switch (subclassCode){
                case 0x03:
                    switch (progIF){
                        case 0x00:
                            return "UHCI Controller";
                        case 0x10:
                            return "OHCI Controller";
                        case 0x20:
                            return "EHCI (USB2) Controller";
                        case 0x30:
                            return "XHCI (USB3) Controller";
                        case 0x80:
                            return "Unspecified";
                        case 0xFE:
                            return "USB Device (Not a Host Controller)";
                    }
            }    
    }
    return to_hstring(progIF);
}