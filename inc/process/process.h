#ifndef __UNIFIED_OS_PROCESS_PROCESS_H
#define __UNIFIED_OS_PROCESS_PROCESS_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Processes{
        struct CPUState
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
        } __attribute__((packed));
        

        class Process{
            friend class ProcessManager;
        private:
            uint8_t* stack; //4KiB
            CPUState* cpu;

        public:
            Process(uint64_t entrypoint);
            //Add a ELF version
            ~Process();
        };

        class ProcessManager{
        private:
            Process* processes[256];
            int processCount;
            int currentProcess;
        
        public:
            ProcessManager();
            ~ProcessManager();

            bool AddProcess(Process* process);

            CPUState* Schedule(CPUState* cpu);
        };
    }
}

#endif