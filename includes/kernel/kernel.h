#ifndef KERNEL_H
#define KERNEL_H

#include <multiboot.h>
#include <kernel/tty/tty.h>
#include <stdio.h>
#include <kernel/mm/physical_memory.h>
#include <kernel/mm/paging.h>
#include <kernel/mm/heap.h>


void kernel_main(multiboot_info_t* mbi);

#endif //KERNEL_H