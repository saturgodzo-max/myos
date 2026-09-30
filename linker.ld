ENTRY(_start)

SECTIONS {
    . = 0x100000; /* GRUB загрузит ядро по этому адресу в памяти (1 МБ) */

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss) }
}
