#ifndef __UNIFIED_OS_INTERRUPTS_INTERRUPTS_H
#define __UNIFIED_OS_INTERRUPTS_INTERRUPTS_H

#include <common/stdint.h>
#include <IO/port.h>

namespace UnifiedOS{
    namespace Interrupts{
        //For the handler
        class InterruptManager;

        struct InterruptRegistersStack
        {
            uint64_t r15;
            uint64_t r14;
            uint64_t r13;
            uint64_t r12;
            uint64_t r11;
            uint64_t r10;
            uint64_t r9;
            uint64_t r8;
            uint64_t rbp;
            uint64_t rdi;
            uint64_t rsi;
            uint64_t rdx;
            uint64_t rcx;
            uint64_t rbx;
            uint64_t rax;
            uint64_t rip;
            uint64_t cs;
            uint64_t rflags;
            uint64_t rsp;
            uint64_t ss;
        };

        //This is passed into assembly when loaded to the system
        struct InterruptDescriptorTablePointer
        {
            uint16_t Limit;
            uint64_t Offset;
        } __attribute__((packed));

        class InterruptHandler{
        protected:  
            //Interupt Stuff
            uint8_t interrupt;
            InterruptManager* interruptManager;

            //So a handler cant be created anywhere
            InterruptHandler(uint8_t interruptNumber, InterruptManager* Manager);
            ~InterruptHandler();
        public:
            //This gets run when the interrupt number is called
            virtual void HandleInterrupt(uint64_t rsp);
        };

        class InterruptManager{
            friend class InterruptHandler;
        protected:
            //Stays constant and allows multiple interrupt managers
            static InterruptManager* ActiveInterruptManager;

            //The handlers from above to handle interrupts
            InterruptHandler* handlers[256];

            //The values of the interrupt (128Bytes)
            //The entry (base) is split up into 3 sections
            struct GateDescriptor
            {
                uint16_t base_low;
                uint16_t selector;
                uint8_t ist;
                uint8_t flags;
                uint16_t base_med;
                uint32_t base_high;
                uint32_t null;
            } __attribute__((packed));

            //The descriptors that get loaded (256 max)
            static GateDescriptor interruptDescriptorTable[256];

            //Not important, well quite useless just a definition of 0x20
            uint16_t hardwareInterruptOffset;

            //Sets and entry in the interruptDescriptorTable
            static void SetInterruptDescriptorTableEntry(uint8_t interrupt, uint64_t handler, uint16_t selector, uint8_t flags, uint8_t ist = 0);

            //Interrupts
                static void InterruptIgnore();

                static void HandleInterruptRequest0();
                static void HandleInterruptRequest1();
                static void HandleInterruptRequest2();
                static void HandleInterruptRequest3();
                static void HandleInterruptRequest4();
                static void HandleInterruptRequest5();
                static void HandleInterruptRequest6();
                static void HandleInterruptRequest7();
                static void HandleInterruptRequest8();
                static void HandleInterruptRequest9();
                static void HandleInterruptRequest10();
                static void HandleInterruptRequest11();
                static void HandleInterruptRequest12();
                static void HandleInterruptRequest13();
                static void HandleInterruptRequest14();
                static void HandleInterruptRequest15();
                static void HandleInterruptRequest49();

                static void HandleInterruptRequest128();

                static void HandleInterruptRequest253();

                static void HandleException0();
                static void HandleException1();
                static void HandleException2();
                static void HandleException3();
                static void HandleException4();
                static void HandleException5();
                static void HandleException6();
                static void HandleException7();
                static void HandleException8();
                static void HandleException9();
                static void HandleException10();
                static void HandleException11();
                static void HandleException12();
                static void HandleException13();
                static void HandleException14();
                static void HandleException15();
                static void HandleException16();
                static void HandleException17();
                static void HandleException18();
                static void HandleException19();
            //

            //Static Handler (Ran in assembly on an interrupt), will call DoHandleInterrupt
            static void HandleInterrupt(uint8_t interrupt, uint64_t rsp);

            //Calls for the specific handler
            void DoHandleInterrupt(uint8_t interrupt, uint64_t rsp);

            //Port for the PIC
            IO::Port8BitSlow PICMasterCommandPort;
            IO::Port8BitSlow PICMasterDataPort;
            IO::Port8BitSlow PICSlaveCommandPort;
            IO::Port8BitSlow PICSlaveDataPort;
            bool PICToggled = true;

        public:
            //Constructors
            InterruptManager();
            ~InterruptManager();
            
            //Return 0x20
            uint16_t HardwareInterruptOffset();

            //Activate/Deactivate the interrupts
            void Activate();
            void Deactivate();

            void DissablePIC();
        };

        //What gets loaded to the system and global for smp
        extern InterruptDescriptorTablePointer idt_pointer;
    }
}

#endif