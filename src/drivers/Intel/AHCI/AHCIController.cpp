#include <drivers/Intel/AHCI/AHCI.h>

#include <paging/PageTableManager.h>

#include <common/stdio.h>

#include <common/cstring.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Devices;
using namespace UnifiedOS::Drivers;
using namespace UnifiedOS::Drivers::AHCI;
using namespace UnifiedOS::Paging;

//Port checker
//Returns the type of a port
AHCIPort::PortType CheckPortType(HBAPort* port){
    //Staus
    uint32_t sataStatus = port->sataStatus;

    //Device Management
    uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
    uint8_t deviceDetection = sataStatus & 0b111;

    //If Not Present Return No Type
    if (deviceDetection != HBA_PORT_DEV_PRESENT) return AHCIPort::PortType::None;
    if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return AHCIPort::PortType::None;

    //Look for a type in the signature
    switch (port->signature){
        case SATA_SIG_ATAPI:
            return AHCIPort::PortType::SATAPI;
        case SATA_SIG_ATA:
            return AHCIPort::PortType::SATA;
        case SATA_SIG_PM:
            return AHCIPort::PortType::PM;
        case SATA_SIG_SEMB:
            return AHCIPort::PortType::SEMB;
        default:
            AHCIPort::PortType::None;
    }
}

//Setup
AHCIDriver::AHCIDriver(UnifiedOS::Devices::PCIDeviceHeader* pciBaseAddress){
    PCIBaseAddress = pciBaseAddress;
    
    //Name of driver
    DriverName = "ACPI 1.0 Driver";

    //For Referencing
    MainObject = this;

    //Port Count Init
    portCount = 0;
}
AHCIDriver::~AHCIDriver(){

}

//Look for ports in the controller
void AHCIDriver::ProbePorts(){
    //Reads from BAR
    uint32_t portsImplemented = ABAR->portsImplemented;

    //32 Max
    for(int i = 0; i < 32; i++){
        //If a port exists exists
        if(portsImplemented & (1 << i)){
            //Get the port type
            AHCIPort::PortType portType = CheckPortType(&ABAR->ports[i]);
            
            //If the port is SATA or SATAPI we create a access
            if(portType == AHCIPort::PortType::SATA /*|| portType == AHCIPort::PortType::SATAPI*/){
                //Intitalise Port
                Ports[portCount] = new AHCIPort(portCount, &ABAR->ports[i], portType);
                Ports[portCount]->portType = portType;
                portCount++;
            }
        }
    }
}

//Driver stuff
void AHCIDriver::Activate(){
    //BAR
    ABAR = (HBAMemory*)((PCIHeader0*)PCIBaseAddress)->BAR5;

    //Map bar in the PTM
    __PAGING__PTM_GLOBAL.MapMemory(ABAR, ABAR);

    //Check For Ports
    ProbePorts();
}
int AHCIDriver::Reset(){return 0;} //TO BE IMPLEMENTED
void AHCIDriver::Deactivate(){} //I dont even know where to begin with this