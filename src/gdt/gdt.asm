[bits 64]
GLOBAL LoadGDT
LoadGDT:
    ;Load
    lgdt [rdi]

    ;Kernel Data
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;Kernel Code
    pop rdi
    mov rax, 0x08
    push rax
    push rdi

    ;Return
    retfq