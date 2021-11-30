#include <smp/smp.h>
#include <interrupts/timer/pit.h>

#include <IO/APIC/apic.h>

#include <memory/heap.h>
#include <memory/memory.h>

#include <gdt/gdt.h>

#include <paging/PageFrameAllocator.h>
#include <paging/PageTableManager.h>

using namespace UnifiedOS;
using namespace UnifiedOS::CPU;
using namespace UnifiedOS::SMP;
using namespace UnifiedOS::IO::APIC;
using namespace UnifiedOS::Interrupts::Timer;
using namespace UnifiedOS::Memory;
using namespace UnifiedOS::Paging;

#include <common/stdio.h>
#include <common/cstring.h>

#include "smpdefines.inc"

//smpdefinces.inc to real pointers
volatile uint16_t* smpMagic = (uint16_t*)SMP_TRAMPOLINE_DATA_MAGIC;
volatile uint16_t* smpID = (uint16_t*)SMP_TRAMPOLINE_CPU_ID;
GlobalDescriptorTable::GDTDescriptor* smpGDT = (GlobalDescriptorTable::GDTDescriptor*)SMP_TRAMPOLINE_GDT_PTR;
volatile uint64_t* smpCR3 = (uint64_t*)SMP_TRAMPOLINE_CR3;
volatile uint64_t* smpStack = (uint64_t*)SMP_TRAMPOLINE_STACK;
volatile uint64_t* smpEntry2 = (uint64_t*)SMP_TRAMPOLINE_ENTRY2;

//ASM for the boot SMP entry (Encoded using the python script)
const char* SMP_ENTRY_ASM = "\xfa\xfc\xb8\x3f\xb3\xa3\x0\x10\xf\x20\xe0\x66\x83\xc8\x20\xf\x22\xe0\x66\xa1\x20\x10\xf\x22\xd8\x66\xb9\x80\x0\x0\xc0\xf\x32\x66\xd\x0\x1\x0\x0\xf\x30\xf\x20\xc0\x66\xd\x1\x0\x0\x80\xf\x22\xc0\xf\x1\x16\x10\x10\xea\x40\x20\x8\x0\xf4\x66\xb8\x10\x0\x8e\xd8\x8e\xc0\x8e\xe0\x8e\xe8\x8e\xd0\x48\x8b\x24\x25\x28\x10\x0\x0\xf\x20\xc0\x66\x83\xe0\xfb\x66\x83\xc8\x2\xf\x22\xc0\xf\x20\xe0\x66\xd\x0\x6\xf\x22\xe0\x48\x31\xed\x48\x8b\x3c\x25\x2\x10\x0\x0\xff\x14\x25\x30\x10\x0\x0\xfa\xf4";
const uint8_t SMP_ENTRY_ASM_LEN = 130;

//CPU Setup
UnifiedOS::CPU::CPU* SMP::CPUs[256] = {0};
uint8_t SMP::ActiveCPUs = 0;
GlobalDescriptorTable::TSS SMP::tss1 __attribute__((aligned(16)));

//End setup
volatile bool doneInit = false;

//Loaded after the cpu runs the smptrampoline
void SMPEntry(uint16_t id) {
    //ID
    UnifiedOS::CPU::CPU* cpu = CPUs[id];
    cpu->currentProcess = nullptr;
    cpu->QueueLock = 0;

    //Set Local cpu (For scheduling to work out what process is to be changed)
    SetCPULocal(cpu);

    //Global descriptor table copy
    cpu->gdt = __PAGING__PFA_GLOBAL.RequestPage(); // kmalloc(GDT64Pointer64.limit + 1); // Account for the 1 subtracted from limit
    memcpy(cpu->gdt, (void*)(GlobalDescriptorTable::__GDTDesc.Offset), GlobalDescriptorTable::__GDTDesc.Size + 1); // Make a copy of the GDT
    cpu->gdtPtr = {GlobalDescriptorTable::__GDTDesc.Size, (uint64_t)cpu->gdt};

    //Load it
    asm volatile("lgdt (%%rax)" ::"a"(&cpu->gdtPtr));

    //Reload IDT
    asm ("lidt %0" : : "m" (Interrupts::idt_pointer));

    //TSS setup
    GlobalDescriptorTable::InitTSS(&cpu->tss, &cpu->gdtPtr);

    //LAPIC toggle
    Enable();

    //Empty the queue
    cpu->Queue = new Vector<Processes::Process*>();

    //Allow next cpu to boot
    doneInit = true;

    //Start interrupts
    asm("sti");

    //Paging
    asm("mov %0, %%cr3" : : "r" (Paging::__PAGING__PTM_GLOBAL.PML4));

    //Print boot
    printf("CPU: ");
    printf(to_string((int64_t)cpu->id));
    printf(" Now Booted\n");

    for (;;){
        
    }

}

//Setup asm
void PrepareTrampoline() {
    //Copy the SMP ASM to a 16bit address to be booted into
    Paging::__PAGING__PTM_GLOBAL.MapMemory((void*)SMP_TRAMPOLINE_ENTRY, (void*)SMP_TRAMPOLINE_ENTRY);
    memcpy((void*)SMP_TRAMPOLINE_ENTRY, SMP_ENTRY_ASM, SMP_ENTRY_ASM_LEN);
}

//Setup a cpu
int InitializeCPU(uint16_t id) {
    //Setup of global cpu
    UnifiedOS::CPU::CPU* cpu = new UnifiedOS::CPU::CPU;
    cpu->id = id;
    cpu->QueueLock = 0;
    CPUs[id] = cpu;

    //Definitions of the cpu setup to link to
    *smpMagic = 0;                                          // Set magic to 0
    *smpID = id;                                            // Set ID to our CPU's ID
    *smpEntry2 = (uint64_t)SMPEntry;                        // Our second entry point
    *smpStack = (uint64_t)__PAGING__PFA_GLOBAL.RequestPage(); // 4K stack
    *smpStack += 0x1000;
    *smpGDT = GlobalDescriptorTable::__GDTDesc;

    //Paging
    asm volatile("mov %%cr3, %%rax" : "=a"(*smpCR3));

    //Send a Init Interrupt
    IO::APIC::SendIPI(id, ICR_DSH_DEST, ICR_MESSAGE_TYPE_INIT, 0);

    //Sleep for 20 ms
    __TIMER__PIT__->SleepM(20);

    //Send the startup IPI
    if ((*smpMagic) != 0xB33F) { // Check if the trampoline code set the flag to let us know it has started
        IO::APIC::SendIPI(id, ICR_DSH_DEST, ICR_MESSAGE_TYPE_STARTUP, (SMP_TRAMPOLINE_ENTRY >> 12));

        __TIMER__PIT__->SleepM(80);
    }

    //Wait for longer
    if ((*smpMagic) != 0xB33F) {
        __TIMER__PIT__->SleepM(100);
    }

    //This will only happen if the asm does not change the smpMagic (Showing a fail)
    if ((*smpMagic) != 0xB33F) {
        //Failed to start
        printf("Failed To Intialise CPU\n");
        return 0;
    }

    //Wait for SMP entry to finish
    while (!doneInit)
        ;

    doneInit = false;

    return 1;
}

void SMP::Intitialise(){
    CPUs[IO::APIC::GetLApicID()] = new CPU::CPU; // Initialize CPU 0 (Current)
    CPUs[IO::APIC::GetLApicID()]->id = IO::APIC::GetLApicID();
    CPUs[IO::APIC::GetLApicID()]->gdt = (void*)&GlobalDescriptorTable::__GDTDesc.Offset;
    CPUs[IO::APIC::GetLApicID()]->gdtPtr = GlobalDescriptorTable::__GDTDesc;
    CPUs[IO::APIC::GetLApicID()]->currentProcess = nullptr;
    CPUs[IO::APIC::GetLApicID()]->QueueLock = 0;
    CPUs[IO::APIC::GetLApicID()]->Queue = new Vector<Processes::Process*>();
    SetCPULocal(CPUs[IO::APIC::GetLApicID()]);

    //Prepare ASM
    PrepareTrampoline();

    ActiveCPUs = 1;

    //Boot other cpus
    for (int i = 0; i < CoreCount; i++) {
        if (LAPIC_IDs[i] != IO::APIC::GetLApicID()){
            if(InitializeCPU(LAPIC_IDs[i]))
                ActiveCPUs++;
        }
    }

    //Main TSS (Done after we copy the GDT to stop issues)
    GlobalDescriptorTable::InitTSS(&CPUs[IO::APIC::GetLApicID()]->tss, (GlobalDescriptorTable::GDTDescriptor*)(&CPUs[IO::APIC::GetLApicID()]->gdtPtr));
}