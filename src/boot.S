
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)


.section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss
    .align 16
    .stack_bottom:
        .skip 16384
    .stack_top:



.section .text
    .global _start
    .type _start, @function
        _start:
            cli

            lgdt gdt_descriptor

            inb $0x64, %al

        .loop_a20:
            test $0x2, %al
            jnz .loop_a20
            mov $0xd1, %al
            outb %al, $0x64
        .loop_a20_2:
            inb $0x64, %al
            test $0x2, %al
            jnz .loop_a20_2
            mov $0xdf, %al
            outb %al, $0x60

            //启用保护模式
            movl %cr0, %eax
            orl $0x1, %eax
            movl %eax, %cr0
            
            ljmp $0x08, $protected_mode_start

        protected_mode_start:
            movl $stack_top, %esp

            call kernel_main
            cli
        
        1:	hlt
	        jmp 1b
.align 8
gdt_start:
    .quad 0x0000000000000000  // Null descriptor
    .quad 0x00cf9a000000ffff  // Code segment descriptor
    .quad 0x00cf92000000ffff  // Data segment descriptor
gdt_end:

gdt_descriptor:
    .word gdt_end - gdt_start - 1  // GDT大小
    .long gdt_start                // GDT起始地址
