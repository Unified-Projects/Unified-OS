#include <IO/DeviceManager/DeviceManager.h>

#include <Boot/bootinfo.h>
#include <memory/heap.h>

using namespace UnifiedOS;

using namespace UnifiedOS::Boot;
using namespace UnifiedOS::Boot::ACPI;

using namespace UnifiedOS::Devices;
using namespace UnifiedOS::Devices::ACPI;

#include <common/stdio.h>

void DeviceManager::LocateACPI(){
    //Headers
    xsdt = (SDTHeader*)(__BOOT__BootContext__->rsdp->XSDTAddress);
    mcfg = (MCFGHeader*)FindTable(xsdt, (char*)"MCFG");
}
void DeviceManager::SetupPCI(Drivers::DriverManager* DriverLoader){
    //Setup PCI devices
    pci = new PCI(mcfg, DriverLoader);
}

DeviceManager::DeviceManager(Drivers::DriverManager* DriverLoader){
    //Setup ACPI
    LocateACPI();

    //PCI
    if(mcfg != NULL){
        SetupPCI(DriverLoader);
    }
}