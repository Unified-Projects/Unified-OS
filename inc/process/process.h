#ifndef __UNIFIED_OS_PROCESS_PROCESS_H
#define __UNIFIED_OS_PROCESS_PROCESS_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Scheduling{
        class Scheduler;
    }

    namespace Processes{
        struct ProcessContext
        {
            uint64_t cr3, rip, rflags, reserved1; // offset 0x00
            uint64_t cs, ss, fs, gs; // offset 0x20
            uint64_t rax, rbx, rcx, rdx, rdi, rsi, rsp, rbp; // offset 0x40
            uint64_t r8, r9, r10, r11, r12, r13, r14, r15; // offset 0x80
            uint8_t fxsave_area[512]; // offset 0xc0
        } __attribute__((packed));
        
        class Process{
            friend Scheduling::Scheduler;
        public:
            static const int DefaultLevel = 1;
            static const size_t DefaultStackBytes = 4096;

        public:
            Process& ContextCreation(uint64_t entry, int64_t data);

            ProcessContext& GetContext();
            uint64_t GetPID() const;
            Process& Sleep();
            Process& Wakeup();

            int GetLevel() const { return Level;}
            bool GetRunning() const { return Running; }

        private:
            uint64_t PID;
            uint8_t* Stack; // 4KiB
            alignas(16) ProcessContext Context;
            uint32_t Level{DefaultLevel};

            //Active
            bool Running{false};

        public:
            Process& SetLevel(int level) {Level = level; return *this;};
            Process& SetRunning(bool running) {Running = running; return *this;}

        protected:
            Process(uint64_t pid);
            ~Process();
        };
    }
}

#endif