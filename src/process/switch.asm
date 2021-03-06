bits 64

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
    ; Dont pop rax yet
%endmacro

global ProcessSwitch
ProcessSwitch:
    mov rsp, rdi ; Set the stack pointer to the location of our register context
    mov rax, rsi ; PML4
    popaq ; Load register context (we don't load RAX yet)

    ; mov cr3, rax ; Set CR3

    pop rax ; Now pop RAX
    iretq ; This will pop RIP, CS, RFLAGS, RSP and SS.