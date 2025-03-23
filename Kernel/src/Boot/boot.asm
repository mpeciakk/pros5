MULTIBOOT_MAGIC             equ 0xe85250d6
MULTIBOOT_ARCHITECTURE      equ 0x00000000
MULTIBOOT_HEADER_LENGTH     equ 0x10

section .multiboot_header
header_start:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_ARCHITECTURE
    dd header_end - header_start
    dd -(MULTIBOOT_MAGIC + MULTIBOOT_ARCHITECTURE + (header_end - header_start))

    ; Framebuffer tag
    align 8
    dw 5                ; framebuffer tag type
    dw 0                ; flags (optional)
    dd 20               ; size
    dd 1024             ; width
    dd 768              ; height
    dd 32               ; depth (32-bit color)

    ; Graphics mode tag
    align 8
    dw 6                ; graphics mode tag type
    dw 0                ; flags (optional)
    dd 12               ; size
    dd 0                ; mode type (0 = linear graphics)
    dd 0                ; width (0 = don't care)
    dd 0                ; height (0 = don't care)
    dd 0                ; depth (0 = don't care)

    ; End tag
    align 8
    dw 0                ; end tag type
    dw 0                ; flags
    dd 8                ; size
header_end:

section .bss
align 16
stack_bottom:
    resb 16384 ; 16KB
stack_top:

section .text
global _loader
extern kmain
_loader:
    ; Set up the stack
    mov esp, stack_top
    
    ; Push multiboot info
    push ebx ; multiboot magic number
    push eax ; multiboot info structure
    
    ; Call the main kernel function
    call kmain

    ; Halt the CPU
    cli
loopb:
    hlt
    jmp loopb
