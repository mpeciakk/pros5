IRQ_BASE equ 0x20

global IDTFlush
IDTFlush:
    mov eax, [esp+4]
    lidt [eax]
    ret

%macro EXCEPTION_NOERRCODE 1
    global isr%1
    isr%1:
        cli
        push dword 0 ; error code
        push dword %1
        jmp exception
%endmacro

%macro EXCEPTION_ERRCODE 1
    global isr%1
    isr%1:
        cli
        push dword %1
        jmp exception
%endmacro

%macro INTERRUPT 1
    global irq%1
    irq%1:
        cli
        push dword 0
        push dword %1+0x20
        jmp interrupt
%endmacro

EXCEPTION_NOERRCODE 0
EXCEPTION_NOERRCODE 1
EXCEPTION_NOERRCODE 2
EXCEPTION_NOERRCODE 3
EXCEPTION_NOERRCODE 4
EXCEPTION_NOERRCODE 5
EXCEPTION_NOERRCODE 6
EXCEPTION_NOERRCODE 7
EXCEPTION_ERRCODE 8
EXCEPTION_NOERRCODE 9
EXCEPTION_ERRCODE 10
EXCEPTION_ERRCODE 11
EXCEPTION_ERRCODE 12
EXCEPTION_ERRCODE 13
EXCEPTION_ERRCODE 14
EXCEPTION_NOERRCODE 15
EXCEPTION_NOERRCODE 16
EXCEPTION_NOERRCODE 17
EXCEPTION_NOERRCODE 18
EXCEPTION_NOERRCODE 19
EXCEPTION_NOERRCODE 20
EXCEPTION_NOERRCODE 21
EXCEPTION_NOERRCODE 22
EXCEPTION_NOERRCODE 23
EXCEPTION_NOERRCODE 24
EXCEPTION_NOERRCODE 25
EXCEPTION_NOERRCODE 26
EXCEPTION_NOERRCODE 27
EXCEPTION_NOERRCODE 28
EXCEPTION_NOERRCODE 29
EXCEPTION_NOERRCODE 30
EXCEPTION_NOERRCODE 31
INTERRUPT 0
INTERRUPT 1
INTERRUPT 2
INTERRUPT 3
INTERRUPT 4
INTERRUPT 5
INTERRUPT 6
INTERRUPT 7
INTERRUPT 8
INTERRUPT 9
INTERRUPT 10
INTERRUPT 11
INTERRUPT 12
INTERRUPT 13
INTERRUPT 14
INTERRUPT 15

extern handleInterrupt

exception:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, esp   ; Save current stack pointer
    push esp       ; Push pointer to Registers struct
    call handleInterrupt
    add esp, 4     ; Clean up argument
    mov esp, ebp   ; Restore stack pointer

    pop gs
    pop fs
    pop es
    pop ds
    popa

    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
    
interrupt:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, esp   ; Save current stack pointer
    push esp       ; Push pointer to Registers struct
    call handleInterrupt
    add esp, 4     ; Clean up argument
    mov esp, ebp   ; Restore stack pointer

    pop gs
    pop fs
    pop es
    pop ds
    popa

    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!