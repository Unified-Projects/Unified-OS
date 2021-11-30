#include <interrupts/syscalls.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Interrupts::Syscalls;

//Registers
struct Registers
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

//Arguments
#define SC_ARG0(r) ((r)->rdi)
#define SC_ARG1(r) ((r)->rsi)
#define SC_ARG2(r) ((r)->rdx)
#define SC_ARG3(r) ((r)->r10)
#define SC_ARG4(r) ((r)->r9)
#define SC_ARG5(r) ((r)->r8)

#include <common/stdio.h>

//Syscalls
uint64_t SysPrint(uint64_t rsp){
    printf((const char*)(SC_ARG0((Registers*)rsp)));
    return rsp;
}

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
    SysPrint,
    SysExit,
    SysKill
};

SyscallHandler::SyscallHandler(InterruptManager* im)
    : InterruptHandler(0x80, im)
{

}

void SyscallHandler::HandleInterrupt(uint64_t rsp){
    if(((InterruptRegistersStack*)rsp)->rax >= SYSCALL_COUNT || !SyscallResults[((InterruptRegistersStack*)rsp)->rax]){
        return;
    }

    SyscallResults[((InterruptRegistersStack*)rsp)->rax](rsp);
}