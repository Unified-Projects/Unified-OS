#include <common/stdint.h>
#include <boot/bootinfo.h>

#include <common/stdio.h>
#include <common/cstring.h>

#include <gdt/gdt.h>

#include <paging/PageTableManager.h>
#include <paging/PageFrameAllocator.h>

#include <memory/memory.h>
#include <memory/heap.h>

#include <interrupts/interrupts.h>

#include <drivers/Driver.h>
#include <drivers/Input/PS2KeyboardDriver.h>
#include <drivers/Input/PS2MouseDriver.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Boot;
using namespace UnifiedOS::GlobalDescriptorTable;
using namespace UnifiedOS::Paging;
using namespace UnifiedOS::Memory;
using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Drivers;

//For locking the memory at the kernel
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

class PrintfKeyboardEventHandler : public PS2KeyboardEventHandler{
public:
    void OnKeyDown(char c){
        if(c < 0x80){
            printf("Keyboard 0x");
            printf(to_hstring((uint8_t)c));
            Next();
        }
    }
};

uint8_t MousePointer[] = {
    0b10000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11100000, 0b00000000,
    0b11110000, 0b00000000,
    0b11111000, 0b00000000,
    0b11111100, 0b00000000,
    0b11111110, 0b00000000,
    0b11111111, 0b00000000,
    0b11111111, 0b10000000,
    0b11111000, 0b00000000,
    0b11011000, 0b00000000,
    0b10011100, 0b00000000,
    0b00001100, 0b00000000,
    0b00001100, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
};

class MouseToScreen : public PS2MouseEventHandler{
    uint32_t x, y;

    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];

    bool Drawn = false;
public:
    MouseToScreen(){
        x = __BOOT__BootContext__->framebuffer->Width / 2;
        y = __BOOT__BootContext__->framebuffer->Height / 2;
    }

    void ClearMouse(){
        if (!Drawn) return;

        int xMax = 16;
        int yMax = 16;
        int differenceX = __BOOT__BootContext__->framebuffer->Width - x;
        int differenceY = __BOOT__BootContext__->framebuffer->Height - y;

        if (differenceX < 16) xMax = differenceX;
        if (differenceY < 16) yMax = differenceY;

        for (int yOff = 0; yOff < yMax; yOff++){
            for (int xOff = 0; xOff < xMax; xOff++){
                int bit = yOff * 16 + xOff;
                int byte = bit / 8;
                if ((MousePointer[byte] & (0b10000000 >> (xOff % 8))))
                {
                    if (getPix(x + xOff, y + yOff) == MouseCursorBufferAfter[xOff + yOff *16]){
                        putPix(x + xOff, y + yOff, MouseCursorBuffer[xOff + yOff * 16]);
                    }
                }
            }
        }
    }

    void DrawMouse(){
        int xMax = 16;
        int yMax = 16;
        int differenceX = __BOOT__BootContext__->framebuffer->Width - x;
        int differenceY = __BOOT__BootContext__->framebuffer->Height - y;

        if (differenceX < 16) xMax = differenceX;
        if (differenceY < 16) yMax = differenceY;

        for (int yOff = 0; yOff < yMax; yOff++){
            for (int xOff = 0; xOff < xMax; xOff++){
                int bit = yOff * 16 + xOff;
                int byte = bit / 8;
                if ((MousePointer[byte] & (0b10000000 >> (xOff % 8))))
                {
                    MouseCursorBuffer[xOff + yOff * 16] = getPix(x + xOff, y + yOff);
                    putPix(x + xOff, y + yOff, 0xFFFFFFFF);
                    MouseCursorBufferAfter[xOff + yOff * 16] = getPix(x + xOff, y + yOff);
                }
            }
        }

        Drawn = true;
    }

    void OnMouseMove(int xOff, int yOff){
        ClearMouse();
        
        x += xOff;
        if(x < 0) x = 0;
        if(x >= __BOOT__BootContext__->framebuffer->Width) x = __BOOT__BootContext__->framebuffer->Width - 1;

        y += yOff;
        if(y < 0) y = 0;
        if(y >= __BOOT__BootContext__->framebuffer->Height) y = __BOOT__BootContext__->framebuffer->Height - 1;

        DrawMouse();
    }

    void OnMouseDown(){
      
    }

    void OnMouseUp(){
      
    }
};

// void sysprintf(const char* str){
//     asm("int $0x80" : : "a" (4), "b" (str));
// }
//
// void TaskA(){
//     while (true)
//     {
//         sysprintf("Tast A Test\n");
//     }
// }
//
// void TaskB(){
//     while (true)
//     {
//         sysprintf("Task B Test\n");
//     }
// }

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

extern "C" void kernelMain(BootInfo* bootInfo)
{
    __BOOT__BootContext__ = bootInfo;

    //Blank Screen
    Clear(0x00);

    //testparse();

    //GDT
    GDTDescriptor gdt;
    gdt.Size = sizeof(GDT) - 1;
    gdt.Offset = (uint64_t)&DefaultGDT; 
    LoadGDT(&gdt);
    
    //Memory
    InitialisePaging();

    //Heap
    //We use a high address to not interrupt other addresses
    //Yes this can lead to issues such as what if we reach the heap and overwrite it
    //Im not sure how I can fix that its just how it is. Well I suppose the pages will be locked
    //So its not too much of an issue.
    InitialiseHeap((void*)0x0000100000000000, 0xFF);

    //Interrupts (Default)
    InterruptManager interrupts;

    //Drivers
    DriverManager driverManager;

    //Keyboard Driver
    PrintfKeyboardEventHandler KeyboardHandler;
    PS2KeyboardDriver keyboard(&interrupts, &KeyboardHandler);
    driverManager.AddDriver(&keyboard);

    //Mouse Driver
    MouseToScreen MouseHandler;
    PS2MouseDriver mouse(&interrupts, &MouseHandler);
    driverManager.AddDriver(&mouse);

    //Activate Drivers
    driverManager.ActivateAll();

    //PIT

    //Interrupts activation
    interrupts.Activate();

    printf("Done!\n");

    while(true){
        //printf("n");
        asm("hlt"); //Saves performance
    }
}