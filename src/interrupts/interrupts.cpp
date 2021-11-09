#include <interrupts/interrupts.h>
#include <paging/PageFrameAllocator.h>

#include <common/stdio.h>
#include <common/cstring.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Paging;

//DEFAULT HANDLER

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* Manager){
    interrupt = interruptNumber;
    interruptManager = Manager;
    interruptManager->handlers[interrupt] = this;
}
InterruptHandler::~InterruptHandler(){
    if(interruptManager->handlers[interrupt] == this){
        interruptManager->handlers[interrupt] = 0;
    }
}
uint64_t InterruptHandler::HandleInterrupt(uint64_t rsp){
    if(interrupt == 0x21){
        printf("Keyboard\n");
    }
    return rsp;
}

// VARS

UnifiedOS::Interrupts::InterruptManager::GateDescriptor UnifiedOS::Interrupts::InterruptManager::interruptDescriptorTable[256];
UnifiedOS::Interrupts::InterruptManager* UnifiedOS::Interrupts::InterruptManager::ActiveInterruptManager = 0;
UnifiedOS::Interrupts::InterruptManager::InterruptDescriptorTablePointer UnifiedOS::Interrupts::InterruptManager::idt_pointer = InterruptDescriptorTablePointer();

///

//Sets an entry in the descriptor table
void InterruptManager::SetInterruptDescriptorTableEntry(uint8_t interrupt, uint64_t handler, uint16_t selector, uint8_t flags, uint8_t ist = 0){
    //Splits the handler address into 3 sections (Its just how the system wants it)
    interruptDescriptorTable[interrupt].base_high = (handler >> 32);
    interruptDescriptorTable[interrupt].base_med = (handler >> 16) & 0xFFFF;
    interruptDescriptorTable[interrupt].base_low = handler & 0xFFFF;

    //Flags (Gate type)
    interruptDescriptorTable[interrupt].flags = flags;

    //Code selector (GDT)
    interruptDescriptorTable[interrupt].selector = selector;

    //Null/Ignore
    interruptDescriptorTable[interrupt].null = 0;

    // Interrupt Stack Table (IST)
    interruptDescriptorTable[interrupt].ist = ist & 0x7;
}

InterruptManager::InterruptManager(Processes::ProcessManager* processManager)
    //Setup port numbers
    :   PICMasterCommandPort(0x20),
        PICMasterDataPort(0x21),
        PICSlaveCommandPort(0xA0),
        PICSlaveDataPort(0xA1)
{
    //Processes
    processes = processManager;

    //Initialise the idt_pointer
    idt_pointer.Limit = 256 * sizeof(GateDescriptor) - 1;
    idt_pointer.Offset = (uint64_t)(&interruptDescriptorTable);

    //Default
    this->hardwareInterruptOffset = 0x20;
    
    //Gate types
    const uint8_t IDT_INTERRUPT_GATE = 0x8E; //142
    const uint8_t IDT_CALL_GATE = 0x8C; //140
    const uint8_t IDT_TRAP_GATE = 0x8F; //143

    //Default to ingnore interrupt
    for(uint8_t i = 255; i > 0; --i) //For some reason for me it just did not work going up the array
    {
        SetInterruptDescriptorTableEntry(i, (uint64_t)InterruptIgnore, 0x08, IDT_INTERRUPT_GATE);
        handlers[i] = 0;
    }
    //So we have to do the first last
    SetInterruptDescriptorTableEntry(0, (uint64_t)InterruptIgnore, 0x08, IDT_INTERRUPT_GATE);
    handlers[0] = 0;

    //Entries
        SetInterruptDescriptorTableEntry(0x00, (uint64_t)HandleException0, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x01, (uint64_t)HandleException1, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x02, (uint64_t)HandleException2, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x03, (uint64_t)HandleException3, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x04, (uint64_t)HandleException4, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x05, (uint64_t)HandleException5, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x06, (uint64_t)HandleException6, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x07, (uint64_t)HandleException7, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x08, (uint64_t)HandleException8, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x09, (uint64_t)HandleException9, 0x08, IDT_INTERRUPT_GATE, 2); //Double Fault
        SetInterruptDescriptorTableEntry(0x0A, (uint64_t)HandleException10, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x0B, (uint64_t)HandleException11, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x0C, (uint64_t)HandleException12, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x0D, (uint64_t)HandleException13, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x0E, (uint64_t)HandleException14, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x0F, (uint64_t)HandleException15, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x10, (uint64_t)HandleException16, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x11, (uint64_t)HandleException17, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x12, (uint64_t)HandleException18, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x13, (uint64_t)HandleException19, 0x08, IDT_INTERRUPT_GATE);

        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, (uint64_t)HandleInterruptRequest0, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, (uint64_t)HandleInterruptRequest1, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x02, (uint64_t)HandleInterruptRequest2, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x03, (uint64_t)HandleInterruptRequest3, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x04, (uint64_t)HandleInterruptRequest4, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x05, (uint64_t)HandleInterruptRequest5, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x06, (uint64_t)HandleInterruptRequest6, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x07, (uint64_t)HandleInterruptRequest7, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x08, (uint64_t)HandleInterruptRequest8, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x09, (uint64_t)HandleInterruptRequest9, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A, (uint64_t)HandleInterruptRequest10, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B, (uint64_t)HandleInterruptRequest11, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, (uint64_t)HandleInterruptRequest12, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D, (uint64_t)HandleInterruptRequest13, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E, (uint64_t)HandleInterruptRequest14, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F, (uint64_t)HandleInterruptRequest15, 0x08, IDT_INTERRUPT_GATE);
        SetInterruptDescriptorTableEntry(0x80, (uint64_t)HandleInterruptRequest128, 0x8, 0xEE, 0); //Syscall
    //
    
    //Loads the interrupts
    asm ("lidt %0" : : "m" (idt_pointer));
    
    //PIC intialiser
    // uint8_t MMask, SMask;

    // MMask = PICMasterDataPort.Read();
    // SMask = PICSlaveDataPort.Read();
    PICMasterCommandPort.Write(0x11);
    PICSlaveCommandPort.Write(0x11);

    PICMasterDataPort.Write(0x20);
    PICSlaveDataPort.Write(0x28);

    PICMasterDataPort.Write(0x04);
    PICMasterDataPort.Write(0x02);

    PICMasterDataPort.Write(0x01);
    PICSlaveDataPort.Write(0x01);

    PICMasterDataPort.Write(0x00);
    PICSlaveDataPort.Write(0x00);
    // PICMasterDataPort.Write(MMask);
    // PICSlaveDataPort.Write(SMask);
}

InterruptManager::~InterruptManager(){
    Deactivate(); //Deactivate when finished
}
uint16_t InterruptManager::HardwareInterruptOffset(){
    return hardwareInterruptOffset; //Default 0x20
}
void InterruptManager::Activate(){
    if(ActiveInterruptManager != 0) //Deactivate Current interrupt manager
        ActiveInterruptManager->Deactivate();

    //Set an the active
    ActiveInterruptManager = this;
    asm("sti"); //Start recieving interrupts
}
void InterruptManager::Deactivate(){
    if(ActiveInterruptManager == this) //If this is active
    {
        ActiveInterruptManager = 0; //Deactivate
        
        asm("cli"); //Stop interrupts
    }
}

//Called by the interrupt
uint64_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint64_t rsp){
    if(ActiveInterruptManager != 0){ //If there is an active interrupt
        return ActiveInterruptManager->DoHandleInterrupt(interrupt, rsp); //Return the stack pointer (Context Switching (Multiprocessing))
    }

    return rsp; //Otherwise just return the stack
}

bool GP = false;

//Main interupt handler
uint64_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint64_t rsp){
    if(handlers[interrupt] != 0){ //If a handler exists
        rsp = handlers[interrupt]->HandleInterrupt(rsp); //Call it
    }
    else if(interrupt != hardwareInterruptOffset)
    { //if an interrupt is not handled here print it
        printf("Unhandled Interrupt ");
        printf(to_hstring(interrupt));
        printf("\n");
        //NOTE
        //Dissable in future when i have a graphical display as will be useless
    }
    
    //If it is a hardware interrupt we need to tell the PIC its ended
    if(hardwareInterruptOffset <= interrupt && interrupt <= hardwareInterruptOffset + 16)
    {
        if(hardwareInterruptOffset + 8 <= interrupt)
            PICSlaveCommandPort.Write(0x20); //EOI
        PICMasterCommandPort.Write(0x20); //EOI
    }

    //Return the stack pointer
    return rsp;
}