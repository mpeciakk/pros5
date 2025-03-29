MULTIBOOT_MAGIC             equ 0xe85250d6
MULTIBOOT_ARCHITECTURE      equ 0x00000000
MULTIBOOT_HEADER_LENGTH     equ 0x10
KERNEL_VIRTUAL_BASE         equ 0xC0000000
KERNEL_PAGE_NUMBER          equ (KERNEL_VIRTUAL_BASE >> 22)

section .multiboot_header
header_start:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_ARCHITECTURE
    dd header_end - header_start
    dd -(MULTIBOOT_MAGIC + MULTIBOOT_ARCHITECTURE + (header_end - header_start))

    ; Information request tag
    align 8
    dw 1                ; information request tag type
    dw 1                ; flags (optional)
    dd 16               ; size (2 + 2 + 4 + 8)
    dd 4                ; request basic meminfo
    dd 6                ; request memory map

    ; Framebuffer tag
    align 8
    dw 5                ; framebuffer tag type
    dw 0                ; flags
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

section .data
align 0x1000
boot_page_directory:
    ; identity map first page (4MB)
    dd 0x00000083

    ; zero pages before kernel
    times (KERNEL_PAGE_NUMBER - 1) dd 0

    ; identity map kernel page (4MB)
    dd 0x00000083

    ; pages after kernel
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

section .text
global _loader
extern kmain
_loader:
    ; load our boot page directory
    mov ecx, (boot_page_directory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

    ; enable 4 mb pages
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

    ; enable paging
    mov ecx, cr0
    or ecx, 0x80000001
    mov cr0, ecx

    lea ecx, [start]
    jmp ecx

start:
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
