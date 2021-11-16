#include <interrupts/syscalls.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Interrupts::Syscalls;


//Arguments
#define SC_ARG0(r) ((r)->rdi)
#define SC_ARG1(r) ((r)->rsi)
#define SC_ARG2(r) ((r)->rdx)
#define SC_ARG3(r) ((r)->r10)
#define SC_ARG4(r) ((r)->r9)
#define SC_ARG5(r) ((r)->r8)

#include <common/stdio.h>

//Syscalls
uint64_t SysExit(uint64_t rsp){ //Exit the Current Process
    return rsp;
}

uint64_t SysKill(uint64_t rsp){ //Kill a process at a PID
    return rsp;
}


//
//
//
//
// SYSCALL MANAGER
//
//
//
Syscall SyscallHandler::SyscallResults[SYSCALL_COUNT] = {
    SysExit,
    SysKill
};

SyscallHandler::SyscallHandler(InterruptManager* im)
    : InterruptHandler(0x80, im)
{

}

uint64_t SyscallHandler::HandleInterrupt(uint64_t rsp){
    if(((InterruptRegistersStack*)rsp)->rax >= SYSCALL_COUNT || !SyscallResults[((InterruptRegistersStack*)rsp)->rax]){
        return rsp;
    }

    SyscallResults[((InterruptRegistersStack*)rsp)->rax](rsp);

    return rsp;
}