ENTRY(_start)

SECTIONS {
    . = 0x100000;

    .text BLOCK(4K) : {
        * (.multiboot)
        * (.text)
    }

    .bss BLOCK(4K) : {
        * (COMMON)
        * (.bss)
        stack_top = .;
    }

    .data BLOCK(4k) : {
        * (.data)
    }

    .rodata BLOCK(4K) : {
        * (.rodata)
    }
}