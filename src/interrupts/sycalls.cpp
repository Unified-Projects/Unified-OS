#include <interrupts/syscalls.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Interrupts::Syscalls;

#include <process/Scheduler/Scheduler.h>
#include <fs/VolumeManager.h>

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
#define SC_ARG0(r) ((r)->rdx)
#define SC_ARG1(r) ((r)->rdi)
#define SC_ARG2(r) ((r)->rsi)
#define SC_ARG3(r) ((r)->r10)
#define SC_ARG4(r) ((r)->r9)
#define SC_ARG5(r) ((r)->r8)

#include <common/stdio.h>
#include <common/cstring.h>

// uint64_t Sys(uint64_t rsp){ //
//     return rsp;
// }

//Syscalls
uint64_t SysPrint(uint64_t rsp){ //Print on the screen (Update to communcate with process and work out if it has a terminal parent)
    printf((const char*)SC_ARG0((Registers*)rsp));
    return rsp;
}


//Process Introduction
uint64_t SysExec(uint64_t rsp){ // Create a process from a ELF
    return rsp;
}

uint64_t SysFork(uint64_t rsp){ // Clone a process
    return rsp;
}

uint64_t SysYeild(uint64_t rsp){ // Stop a process temporaraly
    return rsp;
}

uint64_t SysExit(uint64_t rsp){ //Exit the Current Process
    return rsp;
}

uint64_t SysKill(uint64_t rsp){ //Kill a process at a PID
    return rsp;
}

//Filesystem
uint64_t SysResolveFile(uint64_t rsp){ // Get a file from a path
    //First Resolve the path
    FileSystem::GeneralFile* File = new FileSystem::GeneralFile(FileSystem::__FS_VOLUME_MANAGER__->ResolveFile((const char*)SC_ARG0((Registers*)rsp)));

    //Set the return value
    ((Registers*)rsp)->rax = File;

    return rsp;
}

uint64_t SysResolveDir(uint64_t rsp){ // Get a directory from a path
    return rsp;
}

uint64_t SysCreateFile(uint64_t rsp){ // Create a file
    return rsp;
}

uint64_t SysCreateDir(uint64_t rsp){ // Create a dir
    return rsp;
}

uint64_t SysUpdateFile(uint64_t rsp){ // Update a files data
    return rsp;
}

uint64_t SysUpdateDir(uint64_t rsp){ // Update a directories data
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

    SysExec,
    SysFork,
    SysYeild,
    SysExit,
    SysKill,

    SysResolveFile,
    SysResolveDir,
    SysCreateFile,
    SysCreateDir,
    SysUpdateFile,
    SysUpdateDir,
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