#include "kernelUtil.h"
#include "../gdt/gdt.h"
#include "../interrupts/IDT.h"
#include "../interrupts/interrupts.h"
#include "../IO/IO.h"
#include "../memory/heap.h"

//Page table manager
KernelInfo kernelInfo;

//Prepare PTM
void PrepareMemory(BootInfo* bootInfo){
    //Entries (Pages)
	uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->DescriptorSize;
	
    //Allocator for recieving pages
	GlobalAllocator = PageFrameAllocator();
	GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->DescriptorSize);

    //Get kernel loaded memory size
	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    //Lock the buffer at the kernel to stop being overwritten
	GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    //Get a page
	PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    //Set it to zero
	memset(PML4, 0, 0x1000);

    //Load the Page to the table manager
	GlobalPTM = PageTableManager(PML4);

    //Map memory addresses
	for(uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->DescriptorSize); t+=0x1000){
		GlobalPTM.MapMemory((void*)t, (void*)t);
	}

    //Lock Framebuffer pages
	uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
	uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
	GlobalAllocator.LockPages((void*)fbBase, fbSize / 0x1000 + 1);

    //Map framebuffer memory addresses
	for(uint64_t t = fbBase; t < fbBase + fbSize; t+=4096){
		GlobalPTM.MapMemory((void*)t, (void*)t);
	}

    //Load Page Table Manager to memory
	asm("mov %0, %%cr3" : : "r" (PML4));

    //Publicise it
    kernelInfo.pageTableManager = &GlobalPTM;
}

//Interrupt creator
IDTR idtr;
void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector){
    IDTDescEntry* interrupt = (IDTDescEntry*)(idtr.Offset + entryOffset * sizeof(IDTDescEntry));
    interrupt->SetOffset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}

//
void PrepareInterrupts(){
    //Ammount of Interrupts
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    //Type of interrupts (No TRAPS or GATES yet)
    SetIDTGate((void*)Div0, 0x0, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)BoundRangeExceeded, 0x5, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)InvalidOpcode, 0x6, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)DevNotAvailable, 0x7, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)InvalidTSS, 0xA, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)SegmentNotPresent, 0xB, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)StackSegmentFault, 0xC, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)x87FloatingPoint, 0x10, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)AllignmentCheck, 0x11, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)MachineCheck, 0x12, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)SIMDFloatingPoint, 0x13, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)VirtulizationError, 0x14, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)SecurityException, 0x1E, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)TrippleFault, 0x1E, IDT_TA_InterruptGate, 0x08);

    SetIDTGate((void*)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)PitInterrupt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)KeyboardInterrupt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void*)MouseInterrupt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);

    //Load IDT's
    asm ("lidt %0" : : "m" (idtr));

    //Timer
	RemapPIC();
}

//ACPI Devices
void PrepareACPI(BootInfo* bootInfo){
    //Headers
    ACPI::SDTHeader* xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
    ACPI::MCFGHeader* mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");

    //Setup PCI devices
    PCI::EnumeratePCI(mcfg);
}

//Rendering (Post Intitialised)
BasicRenderer r = BasicRenderer(NULL, NULL);
KernelInfo InitializeKernel(BootInfo* bootInfo){

    //Setup Renderer
    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
    GlobalRenderer = &r;

    //Global Descriptor Table
    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    //Memory Setup
    PrepareMemory(bootInfo);

    //Clear Framebuffer
    memset(bootInfo->framebuffer->BaseAddress, 0, bootInfo->framebuffer->BufferSize);


    //
    //
    // Try to implement Dynamic sizing
    //
    //
    //Heap (Modern Memory) Init (If any issues change the size)
    InitializeHeap((void*)0x0000100000000000, 0xFF);

    //Interrupts
    PrepareInterrupts();

    //Input
    PS2MouseIntialize();

    //Devices
    PrepareACPI(bootInfo);

    //Timer Configuration
    outb(PIC1_DATA, 0b11111000);
    outb(PIC2_DATA, 0b11101111);

    //Finish
    asm ("sti");
    
    return kernelInfo;
}