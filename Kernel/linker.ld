ENTRY(_loader)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 0xC0100000; /* Start at 3GB + 1MB */

    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
		*(.multiboot_header)
		*(.text*)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
		*(.rodata*)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
		*(COMMON)
		*(.bss*)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
		*(.data)
	}

	.other : {
        *(.debug*)
        *(.comment*)
    }
}