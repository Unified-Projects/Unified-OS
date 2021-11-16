#include <exceptions/panic.h>

#include <boot/bootinfo.h>

#include <common/cstring.h>
#include <common/stdio.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Boot;
using namespace UnifiedOS::Exceptions;

//NOTE CREATE A HANDLER FOR THIS SO IT KILLS A PROGRAM INSTEAD OF A FULL KERNEL PANIC

//Image (For When Img file not loaded)
int ErrImg[18][54] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

//Exceptions
const char* Messages[19] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check"
};

//Drawing Block sizes (Scaling Image)
void DrawBlock(unsigned int xOff, unsigned int yOff, unsigned int size = 16, unsigned int color = 0xffffffff){
    unsigned int* pixPtr = (unsigned int*)__BOOT__BootContext__->framebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + size; y++){
        for (unsigned long x = xOff; x < xOff + size; x++){
                *(unsigned int*)(pixPtr + x + (y * __BOOT__BootContext__->framebuffer->PixelsPerScanLine)) = color;
        }
    }
}

//Main Panic
void Exceptions::Panic(const char* PanicMessage){
    //Clear screen
    Clear(0x00);

    //Calculate Sizing and offsetting (Center Allignment)
    uint32_t Size = __BOOT__BootContext__->framebuffer->Width / 120;

    //Offset to center error image
    uint32_t MainXOffset = (__BOOT__BootContext__->framebuffer->Width - (Size * 54)) / 2;

    //Offset for kernel panic string and the error message to be centered
    uint32_t Sub1XOffset = strlen("Kernel Panic: ") + strlen(PanicMessage);
    Sub1XOffset = (__BOOT__BootContext__->framebuffer->Width - (8 * Sub1XOffset)) / 2;

    //Offset for the next string to be centered
    uint32_t Sub2XOffset = strlen("We don't know what caused it.");
    Sub2XOffset = (__BOOT__BootContext__->framebuffer->Width - (8 * Sub2XOffset)) / 2;

    //Indent downwards
    OffsetPos(0, Size);

    //Draw image by looping over the array
    for(int y = 0; y < 18; y++){
        for(int x = 0; x < 54; x++){
            //If not empty draw a block
            if(ErrImg[y][x] == 1){
                DrawBlock(x*Size + MainXOffset, y*Size + Size, Size);
            }
        }
        //Next line
        OffsetPos(0, Size);
    }

    //Next line for strings
    OffsetPos(0, Size);

    //Offset x and draw
    SetPosX(Sub1XOffset);
    printf("Kernel Panic: ");
    printf(PanicMessage);
    Next();

    //Offset x and draw
    SetPosX(Sub2XOffset);
    printf("We don't know what caused it.");
    Next();
}

//Panics from an interrupt
void Exceptions::Exception(uint8_t interrupt){
    //Clear screen
    Clear(0x00);

    //Calculate Sizing and offsetting (Center Allignment)
    uint32_t Size = __BOOT__BootContext__->framebuffer->Width / 120;

    //Offset to center error image
    uint32_t MainXOffset = (__BOOT__BootContext__->framebuffer->Width - (Size * 54)) / 2;

    //Offset for kernel panic string and the error message to be centered
    uint32_t Sub1XOffset = strlen("Kernel Panic: ") + strlen(Messages[interrupt]);
    Sub1XOffset = (__BOOT__BootContext__->framebuffer->Width - (8 * Sub1XOffset)) / 2;

    //Offset for the next string to be centered
    uint32_t Sub2XOffset = strlen("We don't know what caused it.");
    Sub2XOffset = (__BOOT__BootContext__->framebuffer->Width - (8 * Sub2XOffset)) / 2;

    //Indent downwards
    OffsetPos(0, Size);

    //Draw image by looping over the array
    for(int y = 0; y < 18; y++){
        for(int x = 0; x < 54; x++){
            //If not empty draw a block
            if(ErrImg[y][x] == 1){
                DrawBlock(x*Size + MainXOffset, y*Size + Size, Size);
            }
        }
        //Next line
        OffsetPos(0, Size);
    }

    //Next line for strings
    OffsetPos(0, Size);

    //Offset x and draw
    SetPosX(Sub1XOffset);
    printf("Kernel Panic: ");
    printf(Messages[interrupt]);
    Next();

    //Offset x and draw
    SetPosX(Sub2XOffset);
    printf("We don't know what caused it.");
    Next();
}