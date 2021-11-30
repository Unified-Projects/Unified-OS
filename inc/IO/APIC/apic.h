#ifndef __UNIFIED_OS_IO_APIC_H
#define __UNIFIED_OS_IO_APIC_H

#include <common/stdint.h>
#include <IO/DeviceManager/ACPI/ACPI.h>
#include <interrupts/interrupts.h>

#define LOCAL_APIC_ID 0x20 // APIC ID Register
#define LOCAL_APIC_VERSION 0x30 // APIC Version Register
#define LOCAL_APIC_TPR 0x80 // Task Priority Register 
#define LOCAL_APIC_APR 0x90 // Arbitration Priority Register
#define LOCAL_APIC_PPR 0xA0 // Processor Priority Register
#define LOCAL_APIC_EOI 0xB0 // End of Interrupt Register
#define LOCAL_APIC_RRD 0xC0 // Remote Read Register
#define LOCAL_APIC_LDR 0xD0 // Logical Destination Register
#define LOCAL_APIC_DFR 0xE0 // Destination Format Register
#define LOCAL_APIC_SIVR 0xF0 // Spurious Interrupt Vector Register
#define LOCAL_APIC_ISR 0x100 // In-service Register
#define LOCAL_APIC_TMR 0x180 // Trigger Mode Register
#define LOCAL_APIC_IRR 0x200 // Interrupt Request Register
#define LOCAL_APIC_ERROR_STATUS 0x280 // Error Status Register
#define LOCAL_APIC_ICR_LOW 0x300 // Interrupt Command Register Low
#define LOCAL_APIC_ICR_HIGH 0x310 // Interrupt Command Register High
#define LOCAL_APIC_LVT_TIMER 0x320 // Timer Local Vector Table Entry
#define LOCAL_APIC_LVT_THERMAL 0x330 // Thermal Local Vector Table Entry
#define LOCAL_APIC_LVT_PERF_MONITORING 0x340 // Performance Local Vector Table Entry
#define LOCAL_APIC_LVT_LINT0 0x350 // Local Interrupt 0 Local Vector Table Entry
#define LOCAL_APIC_LVT_LINT1 0x360 // Local Interrupt 1 Local Vector Table Entry
#define LOCAL_APIC_LVT_ERROR 0x370 // Error Local Vector Table Entry
#define LOCAL_APIC_TIMER_INITIAL_COUNT 0x380 // Timer Initial Count Register
#define LOCAL_APIC_TIMER_CURRENT_COUNT 0x390 // Timer Current Count Register
#define LOCAL_APIC_TIMER_DIVIDE 0x3E0 // Timer Divide Configuration Register

#define LOCAL_APIC_BASE 0xFFFFFFFFFF000

#define ICR_VECTOR(x) (x & 0xFF)
#define ICR_MESSAGE_TYPE_FIXED 0
#define ICR_MESSAGE_TYPE_LOW_PRIORITY (1 << 8)
#define ICR_MESSAGE_TYPE_SMI (2 << 8)
#define ICR_MESSAGE_TYPE_REMOTE_READ (3 << 8)
#define ICR_MESSAGE_TYPE_NMI (4 << 8)
#define ICR_MESSAGE_TYPE_INIT (5 << 8)
#define ICR_MESSAGE_TYPE_STARTUP (6 << 8)
#define ICR_MESSAGE_TYPE_EXTERNAL (7 << 8)

#define ICR_DSH_DEST 0          // Use destination field
#define ICR_DSH_SELF (1 << 18)  // Send to self
#define ICR_DSH_ALL (2 << 18)   // Send to ALL APICs
#define ICR_DSH_OTHER (3 << 18) // Send to all OTHER APICs 

#define IO_APIC_REGSEL 0x00 // I/O APIC Register Select Address Offset
#define IO_APIC_WIN 0x10 // I/O APIC I/O Window Address offset

#define IO_APIC_REGISTER_ID 0x0 // ID Register
#define IO_APIC_REGISTER_VER 0x1 // Version Register
#define IO_APIC_REGISTER_ARB 0x2 // I/O APIC Arbitration ID
#define IO_APIC_RED_TABLE_START 0x10 // I/O APIC Redirection Table Start
#define IO_APIC_RED_TABLE_ENT(x) (0x10 + 2 * x)

#define IO_RED_TBL_VECTOR(x) (x & 0xFF)

#define IPI_HALT 0xFE
#define IPI_SCHEDULE 0xFD

namespace UnifiedOS{
    namespace IO{
        namespace APIC{
            namespace APICEntries{
                struct ProcessorLocalAPIC //0
                {
                    uint8_t ProcessorID;
                    uint8_t APICID;
                    uint32_t Flags; //(bit 0 = Processor Enabled) (bit 1 = Online Capable)
                } __attribute__((packed));

                struct IOAPIC //1
                {
                    uint8_t IOAPICID;
                    uint8_t Reserved;
                    uint32_t IOAPICAddress;
                    uint32_t GlobalSystemInterruptBase;
                } __attribute__((packed));

                struct IOAPICInterruptSoruceOverride //2
                {
                    uint8_t BusSource;
                    uint8_t IRQSource;
                    uint32_t GlobalSystemInterrupt;
                    uint16_t Flags; //See Below
                } __attribute__((packed));

                struct IOAPICNonMaskableInterruptSource //3
                {
                    uint8_t NMISource;
                    uint8_t Reserved;
                    uint16_t Flags; //See Below
                    uint32_t GlobalSystemInterrupt;
                } __attribute__((packed));

                struct LocalAPICNonMaskableInterrupts //4
                {
                    uint8_t ACPIProcessorID; //0xFF Means all processors
                    uint16_t Flags; //See Below
                    uint8_t LINT; //0 or 1
                } __attribute__((packed));

                struct LocalAPICAddressOverride //5
                {
                    uint16_t Reserved;
                    uint64_t PhysicalAddress; //64-Bit address of LocalAPIC
                } __attribute__((packed));

                struct ProcessorLocalx2APIC //9
                {
                    uint16_t Reserved;
                    uint32_t ProcessorLocalx2APICID;
                    uint32_t Flags; //See Below
                    uint32_t ACPIID;
                } __attribute__((packed));

                //FLAGS
                //
                // If (flags & 2) then the interrupts is active when low
                //  Otherwise when high the interrupt is active
                //
                // If (flags & 8) then the interrupt is level-triggered
                //  If not level triggered then whenever the level changes (edge-triggered)
                //      the interrupt is called
                //  If level triggered then the interrupt will always be called and send EOI
                //      Untill the interrupt changes to the other level (high to low)
                //
                
            }

            //APIC
            extern uint8_t LAPIC_IDs[256];
            extern uint8_t CoreCount;
            extern uint64_t LAPIC_PTR;
            extern uint64_t IOAPIC_PTR;

            //ISOs
            extern APICEntries::IOAPICInterruptSoruceOverride* ISOs[256];
            extern uint8_t ISOSize;

            void ReadAPIC();

            //Local APIC
            void LApicInit();
            uint64_t GetLApicID();
            void SendIPI(uint8_t destination, uint32_t dsh /* Destination Shorthand*/, uint32_t type, uint8_t vector);
            void Enable();

            //IOAPIC
            void IntitateIO();
            void MapLegacyIRQ(uint8_t irq);
            
            //Spurious interrup
            class SpuriousInterrupHandler : public Interrupts::InterruptHandler{
            public:
                SpuriousInterrupHandler(Interrupts::InterruptManager* im);

                void HandleInterrupt(uint64_t rsp);
            };
        }
    }
}

//EOI
extern "C" void LocalAPICEOI();

#endif