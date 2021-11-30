#include <IO/APIC/apic.h>
#include <boot/bootinfo.h>
#include <common/stdio.h>
#include <common/cstring.h>

#include <interrupts/interrupts.h>

using namespace UnifiedOS;
using namespace UnifiedOS::IO;
using namespace UnifiedOS::Devices;

uint8_t APIC::LAPIC_IDs[256];
uint8_t APIC::CoreCount = 0;
uint64_t APIC::LAPIC_PTR = 0;
uint64_t APIC::IOAPIC_PTR = 0;

APIC::APICEntries::IOAPICInterruptSoruceOverride* APIC::ISOs[256] = {0};
uint8_t APIC::ISOSize = 0;

void APIC::ReadAPIC(){
    //Read APIC Entry in the XSDTable
    ACPI::MADTHeader* MADT = (ACPI::MADTHeader*)ACPI::FindTable((ACPI::SDTHeader*)Boot::__BOOT__BootContext__->rsdp->XSDTAddress, "APIC");

    if(MADT != nullptr){
        //Its found
        //Default LAPIC (First entry)
        // LAPIC_PTR = (uint64_t)((uint8_t*)MADT + 0x24);
        LAPIC_PTR = MADT->APICAddress;

        //Now read over entries and get the other information
        for(uint8_t* APICT = (((uint8_t*)MADT) + 0x2C); APICT < ((uint8_t*)MADT + MADT->Header.Length); APICT += APICT[1]){
            switch (APICT[0]) //Check entry type
            {
            case 0: { //Processor
                APICEntries::ProcessorLocalAPIC* Processor = (APICEntries::ProcessorLocalAPIC*)(APICT + 2);
                if(Processor->Flags & 0x01){ //Can be turned on
                    LAPIC_IDs[CoreCount++] = Processor->APICID;
                }
                break;
                }
            case 1: { //IOAPIC
                APICEntries::IOAPIC* IOAPIC = (APICEntries::IOAPIC*)(APICT + 2);
                //Set IO APIC address
                IOAPIC_PTR = IOAPIC->IOAPICAddress;
                break;
                }
            case 2: { //IO Interrupt Source Override //Ignored for now
                APICEntries::IOAPICInterruptSoruceOverride* ISO = (APICEntries::IOAPICInterruptSoruceOverride*)(APICT + 2);
                ISOs[ISOSize++] = ISO;
                break;
                }
            case 3: { //IO NMIs //Ignored for now
                APICEntries::IOAPICNonMaskableInterruptSource* NMIs = (APICEntries::IOAPICNonMaskableInterruptSource*)(APICT + 2);
                break;
                }
            case 4: { //Local NMI //Ignored for now
                APICEntries::LocalAPICNonMaskableInterrupts* NMI = (APICEntries::LocalAPICNonMaskableInterrupts*)(APICT + 2);
                break;
                }
            case 5: { //Local APIC Adrress override
                APICEntries::LocalAPICAddressOverride* LAPICAddressOverride = (APICEntries::LocalAPICAddressOverride*)(APICT + 2);
                //Override the Local APIC address
                LAPIC_PTR = LAPICAddressOverride->PhysicalAddress;
                break;
                }
            case 9: { //Processor x2APIC //Ignored for now
                APICEntries::ProcessorLocalx2APIC* x2APIC = (APICEntries::ProcessorLocalx2APIC*)(APICT + 2);
                break;
                }
            }
        }
    }
}

APIC::SpuriousInterrupHandler::SpuriousInterrupHandler(Interrupts::InterruptManager* im)
    : Interrupts::InterruptHandler(0xFF, im)
{

}

void APIC::SpuriousInterrupHandler::HandleInterrupt(uint64_t rsp){
    //Dont do anything
}