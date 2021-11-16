#ifndef __UNIFIED_OS_INTERRUPTS_SYSCALLS_H
#define __UNIFIED_OS_INTERRUPTS_SYSCALLS_H

#include <common/stdint.h>
#include <interrupts/interrupts.h>

#define SYSCALL_COUNT 2

namespace UnifiedOS{
    namespace Interrupts{
        namespace Syscalls{
            class SyscallHandler;

            typedef uint64_t (*Syscall)(uint64_t rsp);

            class SyscallHandler : public InterruptHandler{
            public:
                static Syscall SyscallResults[SYSCALL_COUNT];

                SyscallHandler(InterruptManager* im);

                virtual uint64_t HandleInterrupt(uint64_t rsp);
            };
        }
    }
}

#endif