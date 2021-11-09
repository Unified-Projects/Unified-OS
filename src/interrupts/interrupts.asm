; Not important as i use -f elf64 but just in case
bits 64

; text section for linker
section .text

; Link to c++ Handle interrupt function
extern _ZN9UnifiedOS10Interrupts16InterruptManager15HandleInterruptEhy

; Setup Exceptions using macro to make life easier
%macro HandleException 2
global _ZN9UnifiedOS10Interrupts16InterruptManager%2HandleException%1Ev
_ZN9UnifiedOS10Interrupts16InterruptManager%2HandleException%1Ev:
    mov byte [interruptnumber], %1
    jmp int_bottom
%endmacro


; Setup IRQ using macros to make life easier
%macro HandleInterruptRequest 2
global _ZN9UnifiedOS10Interrupts16InterruptManager%2HandleInterruptRequest%1Ev
_ZN9UnifiedOS10Interrupts16InterruptManager%2HandleInterruptRequest%1Ev:
    mov byte [interruptnumber], %1 + 32
    jmp int_bottom
%endmacro

; Setup Exceptions
HandleException 0, 16
HandleException 1, 16
HandleException 2, 16
HandleException 3, 16
HandleException 4, 16
HandleException 5, 16
HandleException 6, 16
HandleException 7, 16
HandleException 8, 16
HandleException 9, 16
HandleException 10, 17
HandleException 11, 17
HandleException 12, 17
HandleException 13, 17
HandleException 14, 17
HandleException 15, 17
HandleException 16, 17
HandleException 17, 17
HandleException 18, 17
HandleException 19, 17

; Setup IRQ
HandleInterruptRequest 0, 23
HandleInterruptRequest 1, 23
HandleInterruptRequest 2, 23
HandleInterruptRequest 3, 23
HandleInterruptRequest 4, 23
HandleInterruptRequest 5, 23
HandleInterruptRequest 6, 23
HandleInterruptRequest 7, 23
HandleInterruptRequest 8, 23
HandleInterruptRequest 9, 23
HandleInterruptRequest 10, 24
HandleInterruptRequest 11, 24
HandleInterruptRequest 12, 24
HandleInterruptRequest 13, 24
HandleInterruptRequest 14, 24
HandleInterruptRequest 15, 24
HandleInterruptRequest 49, 24

; Syscalls
HandleInterruptRequest 128, 25

; 32-Bit mode has pusha 64 does not
%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

; 32-Bit mode has popa 64 does not
%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

int_bottom:
    ;Load registers to stack
    pushaq

    ; call C++ Handler
    mov rdi, [interruptnumber]
    mov rsi, rsp
    call _ZN9UnifiedOS10Interrupts16InterruptManager15HandleInterruptEhy

    ; add rsp, 5
    mov rsp, rax; Switch the stack

    ;Remove from stack in reverse order to adding
    popaq

    ; add rsp, 4

; This is the entry for the ignore interrupt (will just return)
global _ZN9UnifiedOS10Interrupts16InterruptManager15InterruptIgnoreEv
_ZN9UnifiedOS10Interrupts16InterruptManager15InterruptIgnoreEv:

    iretq

section .data
    interruptnumber: DB 0 ; Variable for the interrupt number to be passed to the int_bottom from handler