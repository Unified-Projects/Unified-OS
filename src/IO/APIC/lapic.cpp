#include <IO/APIC/apic.h>
#include <IO/port.h>

#include <paging/PageTableManager.h>

using namespace UnifiedOS;
using namespace UnifiedOS::IO;
using namespace UnifiedOS::IO::APIC;

//64-Bit
uint64_t ReadBase() {
    uint64_t low;
    uint64_t high;
    asm("rdmsr" : "=a"(low), "=d"(high) : "c"(0x1B));

    return (high << 32) | low;
}

//64-Bit
void WriteBase(uint64_t val) {
    uint64_t low = val & 0xFFFFFFFF;
    uint64_t high = val >> 32;
    asm("wrmsr" ::"a"(low), "d"(high), "c"(0x1B));
}

//Local in 32 bit
static uint32_t LocalApicIn(uint32_t offset)
{
    return *((volatile uint32_t*)(APIC::LAPIC_PTR + offset));
}

//Local out
static void LocalApicOut(uint32_t offset, uint32_t data)
{
    *((volatile uint32_t*)(APIC::LAPIC_PTR + offset)) = data; 
}

//Enable a specific cpu
void APIC::Enable(){
    WriteBase(ReadBase() | (1UL << 11));

    //Enable APIC, Vector 255
    LocalApicOut(LOCAL_APIC_SIVR, LocalApicIn(LOCAL_APIC_SIVR) | 0x1FF);
}

//Init the local apic
void APIC::LApicInit(){
    //Map Address
    Paging::__PAGING__PTM_GLOBAL.MapMemory((void*)APIC::LAPIC_PTR, (void*)APIC::LAPIC_PTR);

    //Enable Current CPU
    Enable();
}

//Get the current CPU ID (Normally 0)
uint64_t APIC::GetLApicID(){
    return LocalApicIn(0x20) >> 24;
}

//Sending and interprocessor interrupt
void APIC::SendIPI(uint8_t destination, uint32_t dsh /* Destination Shorthand*/, uint32_t type, uint8_t vector){
    uint32_t high = ((uint32_t)destination) << 24;
    uint32_t low = dsh | type | ICR_VECTOR(vector);

    LocalApicOut(LOCAL_APIC_ICR_HIGH, high);
    LocalApicOut(LOCAL_APIC_ICR_LOW, low);
}

//The end of interrupt
extern "C" void LocalAPICEOI() { LocalApicOut(LOCAL_APIC_EOI, 0); }