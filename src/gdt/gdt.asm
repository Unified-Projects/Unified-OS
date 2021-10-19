[bits 64]
LoadGDT:   
    ;Load
    lgdt [rdi]
    ;Set registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ;Replace
    pop rdi
    mov rax, 0x08
    push rax
    push rdi
    ;Return
    retfq
GLOBAL LoadGDT