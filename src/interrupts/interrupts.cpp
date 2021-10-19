#include "interrupts.h"
#include "panic.h"
#include "../IO/IO.h"
#include "../userinput/keyboard.h"
#include "../scheduling/pit/pit.h"

__attribute__((interrupt)) void Div0(interrupt_frame* frame){
    
    Panic("Attempted To Divide By Zero!");

    while(true);
}

__attribute__((interrupt)) void BoundRangeExceeded(interrupt_frame* frame){
    
    Panic("Bound Range Exceeded!");

    while(true);
}

__attribute__((interrupt)) void InvalidOpcode(interrupt_frame* frame){
    
    Panic("Invalid Opcode!");

    while(true);
}

__attribute__((interrupt)) void DevNotAvailable(interrupt_frame* frame){
    
    Panic("Device Not Available!");

    while(true);
}

__attribute__((interrupt)) void InvalidTSS(interrupt_frame* frame){
    
    Panic("Invalid TSS!");

    while(true);
}

__attribute__((interrupt)) void SegmentNotPresent(interrupt_frame* frame){
    
    Panic("Segment Not Present!");

    while(true);
}

__attribute__((interrupt)) void StackSegmentFault(interrupt_frame* frame){

    Panic("Stack Segment Fault!");

    while(true);
}

__attribute__((interrupt)) void x87FloatingPoint(interrupt_frame* frame){
    
    Panic("x87 Floating point error!");

    while(true);
}

__attribute__((interrupt)) void AllignmentCheck(interrupt_frame* frame){
    
    Panic("Allignment check!");

    while(true);
}

__attribute__((interrupt)) void MachineCheck(interrupt_frame* frame){
    
    Panic("Machine Check!");

    while(true);
}

__attribute__((interrupt)) void SIMDFloatingPoint(interrupt_frame* frame){
    
    Panic("SIMD Floating Point Exception!");

    while(true);
}

__attribute__((interrupt)) void VirtulizationError(interrupt_frame* frame){
    
    Panic("Virtualization Execption!");

    while(true);
}

__attribute__((interrupt)) void SecurityException(interrupt_frame* frame){
    
    Panic("Security Exception!");

    while(true);
}

__attribute__((interrupt)) void TrippleFault(interrupt_frame* frame){

    Panic("Tripple Fault Detected!");

    while(true);
}


__attribute__((interrupt)) void PageFault_Handler(interrupt_frame* frame){
    
    Panic("Page Fault Detected!");

    while(true);
}

__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame* frame){
    
    Panic("Double Fault Detected!");

    while(true);
}

__attribute__((interrupt)) void GPFault_Handler(interrupt_frame* frame){
    
    Panic("General Protection Fault Detected!");

    while(true);
}

__attribute__((interrupt)) void KeyboardInterrupt_Handler(interrupt_frame* frame){
    uint8_t scancode = inb(0x60);

    HandleKeyboard(scancode);

    PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInterrupt_Handler(interrupt_frame* frame){

    uint8_t mouseData = inb(0x60);

    HandlePS2Mouse(mouseData);

    PIC_EndSlave();
}

__attribute__((interrupt)) void PitInterrupt_Handler(interrupt_frame* frame){
    PIT::Tick();
    PIC_EndMaster();
}

void PIC_EndMaster(){
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

//Mapping
void RemapPIC(){
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
    io_wait();
}