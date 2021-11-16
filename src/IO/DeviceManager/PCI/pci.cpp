#include <IO/DeviceManager/PCI/pci.h>
#include <paging/PageTableManager.h>

#include <common/stdio.h>
#include <common/cstring.h>

using namespace UnifiedOS;

using namespace UnifiedOS::Devices;
using namespace UnifiedOS::Devices::ACPI;

using namespace UnifiedOS::Paging;

//Check PCI device type
void PCI::EnumerateFunction(uint64_t deviceAddress, uint64_t function){
    //Function
    uint64_t offset = function << 12;

    //Address
    uint64_t functionAddress = deviceAddress + offset;
    __PAGING__PTM_GLOBAL.MapMemory((void*)functionAddress, (void*)functionAddress);

    //Device Header creation
    PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)functionAddress;

    //Validate
    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xFFFF) return;

    //DEBUG
    // printf("\n\n");

    // printf("Vendor: ");
    // printf(GetVendorName(pciDeviceHeader->VendorID));
    // printf("\n");

    // printf("Device: ");
    // printf(GetDeviceName(pciDeviceHeader->VendorID, pciDeviceHeader->DeviceID));
    // printf("\n");

    // printf("SubClass: ");
    // printf(GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->SubClass));
    // printf("\n");

    // printf("ProgIF: ");
    // printf(GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->SubClass, pciDeviceHeader->ProgIF));
    // printf("\n");

    //ADD LOAD HERE
    //STILL NEEDS IMPLEMENTING
    EnableDriverAtProgIFName(pciDeviceHeader);
}

//Search for functions within device
void PCI::EnumerateDevice(uint64_t busAddress, uint64_t device){
    //Get offset
    uint64_t offset = device << 15;

    //Get address
    uint64_t deviceAddress = busAddress + offset;
    __PAGING__PTM_GLOBAL.MapMemory((void*)deviceAddress, (void*)deviceAddress);

    //Header at address
    PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)deviceAddress;

    //Validate
    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xFFFF) return;

    //Search for function (Max 8 functions in one device)
    for (uint64_t function = 0; function < 8; function++){
        EnumerateFunction(deviceAddress, function);
    }
}

//Load devices and functions at bus
void PCI::EnumerateBus(uint64_t baseAddress, uint64_t bus){
    //Offset
    uint64_t offset = bus << 20;

    //Get address
    uint64_t busAddress = baseAddress + offset;
    __PAGING__PTM_GLOBAL.MapMemory((void*)busAddress, (void*)busAddress);

    //Device header
    PCIDeviceHeader* pciDeviceHeader = (PCIDeviceHeader*)busAddress;

    //Validation
    if (pciDeviceHeader->DeviceID == 0) return;
    if (pciDeviceHeader->DeviceID == 0xFFFF) return;

    //Look over devices (maximum 32 devices per bus)
    for (uint64_t device = 0; device < 32; device++){
        EnumerateDevice(busAddress, device);
    }
}

PCI::PCI(MCFGHeader* MCFG, Drivers::DriverManager* DriverLoader){
    DriverM = DriverLoader;

    //Get entry count fom the MCFG
    int entries = ((MCFG->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);

    //Look over the entries
    for (int t = 0; t < entries; t++){
        //Device Config
        ACPI::DeviceConfig* newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)MCFG + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));

        //Look over the busses on the entry
        for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++){
            EnumerateBus(newDeviceConfig->BaseAddress, bus);
        }
    }
}