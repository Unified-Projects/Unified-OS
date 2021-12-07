#ifndef __UNIFIED_OS_INTERRUPTS_SYSCALL_H
#define __UNIFIED_OS_INTERRUPTS_SYSCALL_H

#include <common/stdint.h>
#include <interrupts/interrupts.h>

#define SYSCALL_COUNT 999

#define SYS_PRINT 0

#define SYS_EXEC 1
#define SYS_FORK 2
#define SYS_YEILD 3
#define SYS_EXIT 4
#define SYS_KILL 5

inline void* __syscall(uint8_t SyscallNumber, uint64_t ARG0 = 0, uint64_t ARG1 = 0, uint64_t ARG2 = 0, uint64_t ARG3 = 0, uint64_t ARG4 = 0, uint64_t ARG5 = 0){
    void* ToReturn = nullptr;

    __asm__
    (
        "int $0x80\n"
        : "=a" (ToReturn)
        : "r" (ARG0), "r" (ARG1), "r" (ARG2), "r" (ARG3), "r" (ARG4), "r" (ARG5), "a" (SyscallNumber) //Syscall
    ) ;

    return ToReturn;
}

template<typename... T>
inline void* syscall(uint64_t call, T... args){
    return __syscall(call, (uint64_t)(args)...);
}

#endif