#ifndef __UNIFIED_OS_INTERRUPTS_SYSCALLS_H
#define __UNIFIED_OS_INTERRUPTS_SYSCALLS_H

#include <common/stdint.h>
#include <interrupts/interrupts.h>

#define SYSCALL_COUNT 3

namespace UnifiedOS{
    namespace Interrupts{
        namespace Syscalls{
            //Sysycall entry
            typedef uint64_t (*Syscall)(uint64_t rsp);

            //Syscall
            class SyscallHandler : public InterruptHandler{
            public:
                //Each syscall
                static Syscall SyscallResults[SYSCALL_COUNT];

                //Setup
                SyscallHandler(InterruptManager* im);

                //Interrup Handler
                virtual void HandleInterrupt(uint64_t rsp);
            };
        }
    }
}

#endif