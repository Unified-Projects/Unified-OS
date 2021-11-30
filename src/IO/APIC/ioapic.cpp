#include <IO/APIC/apic.h>
#include <boot/bootinfo.h>
#include <common/stdio.h>
#include <common/cstring.h>

#include <interrupts/interrupts.h>

#include <Paging/PageTableManager.h>

using namespace UnifiedOS;
using namespace UnifiedOS::IO;
using namespace UnifiedOS::Devices;

//Location of registers
volatile uint32_t* registerSelect;
//The io window for read/writing
volatile uint32_t* ioWindow;

uint32_t interrupts;
uint32_t apicID;

//Read from register offset
uint32_t Read32(uint32_t reg) {
    *registerSelect = reg;
    return *ioWindow;
}

//Write at register offset
void Write32(uint32_t reg, uint32_t data) {
    *registerSelect = reg;
    *ioWindow = data;
}

//Write 64 bits
void Write64(uint32_t reg, uint64_t data) {
    uint32_t low = data & 0xFFFFFFFF;
    uint32_t high = data >> 32;

    Write32(reg, low);
    Write32(reg + 1, high);
}

//Setup a irq to be sendable to the APIC
void Redirect(uint8_t irq, uint8_t vector, uint32_t delivery) {
    Write64(IO_APIC_RED_TABLE_ENT(irq), delivery | vector);
}

//IO Inti
void APIC::IntitateIO(){
    //Map the IOAPIC ptr
    Paging::__PAGING__PTM_GLOBAL.MapMemory((void*)IOAPIC_PTR, (void*)IOAPIC_PTR);

    //Setuo the IO
    registerSelect = (uint32_t*)(IOAPIC_PTR + IO_APIC_REGSEL);
    ioWindow = (uint32_t*)(IOAPIC_PTR + IO_APIC_WIN);

    //Readt the IDs
    interrupts = Read32(IO_APIC_REGISTER_VER) >> 16;
    apicID = Read32(IO_APIC_REGISTER_ID) >> 24;

    //Pre assigned Interrupts to be loaded
    for(unsigned i = 0; i < ISOSize; i++){
        Redirect(ISOs[i]->GlobalSystemInterrupt, ISOs[i]->IRQSource + 0x20, /*ICR_MESSAGE_TYPE_LOW_PRIORITY*/ 0);
    }
}

//Map a irq manually
void APIC::MapLegacyIRQ(uint8_t irq) {
    for(unsigned i = 0; i < ISOSize; i++){
        if (ISOs[i]->IRQSource == irq)
            return; // We should have already redirected this IRQ
    }

    Redirect(irq, irq + 0x20, ICR_MESSAGE_TYPE_LOW_PRIORITY);
}