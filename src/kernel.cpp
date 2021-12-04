#include <common/stdint.h>
#include <boot/bootinfo.h>

#include <pointers.h>

#include <IO/APIC/apic.h> //COMMENT
#include <smp/smp.h> //COMMENT

#include <gdt/gdt.h>
#include <gdt/tss.h> //COMMENT

#include <paging/PageTableManager.h>
#include <paging/PageFrameAllocator.h>

#include <memory/memory.h>
#include <memory/heap.h>

#include <process/Scheduler/Scheduler.h> //Comment (INCLUDES PROCESSES)

#include <interrupts/interrupts.h>
#include <exceptions/exceptions.h>
#include <interrupts/syscalls.h> //Comment
#include <interrupts/timer/pit.h>

#include <drivers/Driver.h>
#include <drivers/Intel/AHCI/AHCI.h>

#include <IO/DeviceManager/DeviceManager.h> //Comment

#include <fs/VolumeManager.h> //Comment

#include <drivers/Input/PS2KeyboardDriver.h>
#include <drivers/Input/PS2MouseDriver.h>

#include <common/stdio.h>
#include <common/cstring.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Boot;

using namespace UnifiedOS::GlobalDescriptorTable;

using namespace UnifiedOS::Paging;
using namespace UnifiedOS::Memory;

using namespace UnifiedOS::Processes;

using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Exceptions;
using namespace UnifiedOS::Interrupts::Syscalls;
using namespace UnifiedOS::Interrupts::Timer;

using namespace UnifiedOS::Drivers;

using namespace UnifiedOS::Devices;

using namespace UnifiedOS::FileSystem;

//SOMETHING TO HAVE A LOOK INTO FOR MAXIMUM MEMORY EFFICIENCY
//Look over code and make sure all needed * are kept but all un needed get removed
// (delete pointer)
//
//
//
//

//For locking the memory at the kernel
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void InitialisePaging(){
    //Entries (Pages)
	uint64_t mMapEntries = __BOOT__BootContext__->mMapSize / __BOOT__BootContext__->DescriptorSize;

    //Load Memory To Page Frame Allocator
    __PAGING__PFA_GLOBAL = PageFrameAllocator();
    __PAGING__PFA_GLOBAL.ReadEFIMemoryMap(__BOOT__BootContext__->mMap, __BOOT__BootContext__->mMapSize, __BOOT__BootContext__->DescriptorSize);

    uint64_t SizeOfKernel = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t PageCountOfKernel = (uint64_t)SizeOfKernel / 0x1000 + 1;

    //Lock memory pages at kernel positions
    __PAGING__PFA_GLOBAL.LockPages(&_KernelStart, PageCountOfKernel);

    //Get a Page for the Page Table Manager
    PageTable* PML4 = (PageTable*)__PAGING__PFA_GLOBAL.RequestPage();

    //Fill it with zero to stop any issues with default
    memset(PML4, 0, 0x1000);

    //Setup the page table manager
    __PAGING__PTM_GLOBAL = PageTableManager(PML4);

    //Map memory addresses to default
    for(uint64_t t = 0; t < __PAGING__TotalMemorySize__; t+=0x1000){ //We do this in 4KiB Pages
		__PAGING__PTM_GLOBAL.MapMemory((void*)t, (void*)t);
	}

    //Lock Framebuffer Pages
    uint64_t FramebufferBase = (uint64_t)__BOOT__BootContext__->framebuffer->BaseAddress;
    uint64_t FramebufferSize = (uint64_t)__BOOT__BootContext__->framebuffer->BufferSize + 0x1000; //We add this is a padding

    __PAGING__PFA_GLOBAL.LockPages((void*)FramebufferBase, FramebufferSize / 0x1000 + 1); // +1 just incase not entire fit

    //Map the framebuffer address
    for(uint64_t t = FramebufferBase; t < FramebufferBase + FramebufferSize; t+=4096){ //We do this in 4KiB Pages
		__PAGING__PTM_GLOBAL.MapMemory((void*)t, (void*)t);
	}

    //Load the Page Table
    asm("mov %0, %%cr3" : : "r" (PML4));
}

// inline void WaitSignal() {
//     IO::Port8Bit CommandPort(0x64);
//     int timeout = 10000;
//     while (timeout--)
//         if ((CommandPort.Read() & 0x2) != 0x2)
//             return;
// }
//
// template <bool isMouse> inline void WaitData() {
//     IO::Port8Bit CommandPort(0x64);
//     int timeout = 10000;
//     while (timeout--)
//         if ((CommandPort.Read() & 0x21) == (isMouse ? 0x21 : 0x1))
//             return;
// }
//
// void PS2Init(){
//     IO::Port8Bit DataPort(0x60);
//     IO::Port8Bit CommandPort(0x64);
//     IO::Port8Bit PITMaster(0x20);
//
//     // Start by disabling both ports
//     WaitSignal();
//     CommandPort.Write(0xAD);
//     WaitSignal();
//     CommandPort.Write(0xA7);
//
//     DataPort.Read(); // Discard any data
//
//     WaitSignal();
//     CommandPort.Write(0x20);
//     WaitData<false>();
//     uint8_t status = PITMaster.Read();
//
//     WaitSignal();
//     CommandPort.Write(0xAE);
//     WaitSignal();
//     CommandPort.Write(0xA8);
//
//     // Enable interrupts, enable keyboard and mouse clock
//     status = ((status & ~0x30) | 3);
//     WaitSignal();
//     CommandPort.Write(0x60);
//     WaitSignal();
//     CommandPort.Write(status);
//     WaitData<false>();
//     DataPort.Read();
// }

void InitVolumes(DriverManager* driverM){

    //NOTE
    //TRY TO SETUP WITH LOOKING TO SEE IF THE VOLUME

    //Locate Driver
    Driver* driver = driverM->FindDriver("ACPI 1.0 Driver");

    //Ensure Driver Found
    if(driver != nullptr){
        //Convert to AHCI
        if(driver->MainObject != nullptr){
            Drivers::AHCI::AHCIDriver* AHCIdriver = (Drivers::AHCI::AHCIDriver*)(driver->MainObject);
            //Look at ports
            for(int p = 0; p < AHCIdriver->portCount; p++){
                //Find Disks
                if(AHCIdriver->Ports[p]->portType == AHCI::AHCIPort::PortType::SATA){
                    //Mount
                    __FS_VOLUME_MANAGER__->MountVolume(AHCIdriver->Ports[p]);
                }
            }
        }
    }
}

void KernelStage2(){
    //Volumes
    InitVolumes(Pointers::Drivers::DriverManager);
    //GP FAULT CAUSED HERE... ^
    
    // PS2Init();

    printf("Stage 2 Entry Complete\n");

    while (true)
    {
        /* code */
    }
    
}

extern "C" void kernelMain(BootInfo* bootInfo)
{
    __BOOT__BootContext__ = bootInfo;

    //Blank Screen
    Clear(0x00);

    //Detect SMP cores test
    //I Dont know why (I think a delay effect) but whenever I remove the prints
    //It stops working???
    printf("SMP APIC: \n");
    IO::APIC::ReadAPIC();
    printf("Found ");
    printf(to_string((int64_t)IO::APIC::CoreCount));

    printf(", IOAPIC ");
    printf(to_hstring(IO::APIC::IOAPIC_PTR));

    printf(", LAPIC ");
    printf(to_hstring(IO::APIC::LAPIC_PTR));

    printf(", Processor IDs: ");
    for(int i = 0; i < IO::APIC::CoreCount; i++){
        printf(to_string((int64_t)IO::APIC::LAPIC_IDs[i]));
        printf(" ");
    }
    printf("\n");

    //Memory
    InitialisePaging();

    //GDT
    LoadGDT(&__GDTDesc);

    //Heap
    //We use a high address to not interrupt other addresses
    //Yes this can lead to issues such as what if we reach the heap and overwrite it
    //Im not sure how I can fix that its just how it is. Well I suppose the pages will be locked
    //So its not too much of an issue.
    InitialiseHeap((void*)0x0000100000000000, 0xFF);

    //Interrupts (Default)
    Pointers::Interrupts::Interrupts = new InterruptManager();

    //Syscalls
    Pointers::Interrupts::Syscalls::Syscalls = new SyscallHandler(Pointers::Interrupts::Interrupts);

    //Intialise Exceptions
    Pointers::Exceptions::Exceptions = new ExceptionManager(Pointers::Interrupts::Interrupts);

    //Drivers
    Pointers::Drivers::DriverManager = new DriverManager();

    //Devices
    Pointers::Devices::DeviceManager = new DeviceManager(Pointers::Drivers::DriverManager);

    // //Keyboard Driver MAKE USING new
    // PrintfKeyboardEventHandler KeyboardHandler;
    // PS2KeyboardDriver keyboard(Pointers::Interrupts::Interrupts, &KeyboardHandler);
    // Pointers::Drivers::DriverManager->AddDriver(&keyboard);

    // //Mouse Driver MAKE USING new
    // MouseToScreen MouseHandler;
    // PS2MouseDriver mouse(Pointers::Interrupts::Interrupts, &MouseHandler);
    // Pointers::Drivers::DriverManager->AddDriver(&mouse);

    //Activate Drivers
    //SETUP NOTE: Make it so when a driver is called to activate it check if already ative and ignores if active
    //To allow for more drivers to be loaded after this boot
    Pointers::Drivers::DriverManager->ActivateAll();

    //PIT
    __TIMER__PIT__ = new PIT(Pointers::Interrupts::Interrupts);
    __TIMER__PIT__->SetFrequency(1000); //INACURRATE

    //ERRORS WITH PIT MAPPING ON MODERN HARDWARE

    //Dissable PIC
    Pointers::Interrupts::Interrupts->DissablePIC();

    //APIC NOTE
    //Sometimes the interrupts do not register which is an issue (With PIT)
    //As this will cause the smp to fail to initialise
    //However this only seems to be with qemu not real hardware

    //APIC Inits
    //Spurious Interrupt
    IO::APIC::SpuriousInterrupHandler* SpuriousInterupts = new IO::APIC::SpuriousInterrupHandler(Pointers::Interrupts::Interrupts);
    IO::APIC::LApicInit();
    IO::APIC::IntitateIO();

    IO::APIC::MapLegacyIRQ(0x01); //PS2 Keyboard
    IO::APIC::MapLegacyIRQ(0x0C); //PS2 Mouse

    //Interrupts activation
    Pointers::Interrupts::Interrupts->Activate();

    //SMP
    //Will now boot all the cpu's that are not booted.
    //64-Bit gets toggled with gdt
    //Interrupts are synced
    SMP::Intitialise();
    printf("All ");
    printf(to_string((int64_t)SMP::ActiveCPUs));
    printf(" Have Been Booted!\n\n");

    //Issues here with real hardware:
    //Either SMP fails to exit (I presume TSS)
    //Scheduling has issues with process swapping and all of that swaps

    //Processes
    Scheduling::IntialiseScheduler(Pointers::Interrupts::Interrupts, (uint64_t)KernelStage2); //CAUSES ISSUES (REAL HARDWARE Div by zero Exception)
    // Process* proctest = Scheduling::__SCHEDULER__->NewProcess("TestProcess", (uint64_t)TaskA, 0);

        //TYPES
            //User space (NEED TO IMPLEMENT) (https://wiki.osdev.org/Getting_to_Ring_3)
            //    This will also need to link to system calls for userspace to reach
            //Kernel space (This)

    // KernelStage2();

    while(true){
        // printf("Task Kernel...\n");
        // asm("hlt"); //Saves performance
    }
}