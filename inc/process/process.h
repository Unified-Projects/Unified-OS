#ifndef __UNIFIED_OS_PROCESS_PROCESS_H
#define __UNIFIED_OS_PROCESS_PROCESS_H

#include <common/stdint.h>

#define DEFAUL_PROCESS_TICK_COUNT 10

namespace UnifiedOS{
    namespace Scheduling{
        class Scheduler;
    }

    namespace Processes{
        //Registers for task swapping
        struct ProcessContext
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
            uint64_t rflags; //Verry important that this is 0x202
            uint64_t rsp;
            uint64_t ss;
        } __attribute__((packed));

        //FX_state
        struct FXState{
            uint16_t fcw; // FPU Control Word
            uint16_t fsw; // FPU Status Word
            uint8_t ftw;  // FPU Tag Words
            uint8_t zero; // Literally just contains a zero
            uint16_t fop; // FPU Opcode
            uint64_t rip;
            uint64_t rdp;
            uint32_t mxcsr;      // SSE Control Register
            uint32_t mxcsrMask;  // SSE Control Register Mask
            uint8_t st[8][16];   // FPU Registers, Last 6 bytes reserved
            uint8_t xmm[16][16]; // XMM Registers
        } __attribute__((packed));

        //States (Need to be used and checked!)
        enum ProcessState{
            PROCESS_DYING,
            PROCESS_READY,
            PROCESS_SLEEPING, //Add a sleeping tick count
            PROCESS_RUNNING
        };
        
        //Process
        class Process{
            friend Scheduling::Scheduler;
        public:
            //Stack size
            static const size_t DefaultStackBytes = 0x1000;

        public:
            //Contexts (Registers and that stuff)
            Process* ContextCreation(uint64_t entry, int64_t data);
            Process* IdleContextCreation(uint64_t entry, int64_t data);
            //ELF once fs is done

            //Return the private PID
            uint64_t GetPID() const;

            // void Kill();

        private:
            //ID
            uint64_t PID;

        public:
            //Context of the Process for task swapping
            uint8_t* Stack; // 4KiB
            ProcessContext Context;
            FXState* fx_State;
            uint64_t fsBase = 0;

        public:
            //State
            ProcessState State = PROCESS_READY;

            //Timing
            uint64_t ActiveTicks = 0;
            uint8_t DefaultTimeSlice = DEFAUL_PROCESS_TICK_COUNT;
            uint8_t TimeSlice = DefaultTimeSlice;
            
            //Name
            char Name[16];

        protected:
            //Setup
            Process(uint64_t pid);
            ~Process(); //Clean the stack once done
        };
    }
}

#endif