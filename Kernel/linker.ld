ENTRY(_loader)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 0x100000; /* Start at 1MB */

    .text ALIGN(4K) : {
        *(.multiboot_header)
        *(.text*)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss*)
    }

    .debug : {
        *(.debug*)
    }

    .comment : {
        *(.comment*)
    }
}