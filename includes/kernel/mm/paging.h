#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <kernel/mm/physical_memory.h>
#include <kprintf.h>

#define PAGE_SIZE 4096  
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024

#define PG_PRESENT              (0x1)
#define PG_WRITE                (0x1 << 1)
#define PG_ALLOW_USER           (0x1 << 2)
#define PG_WRITE_THROUGHT       (1 << 3)
#define PG_DISABLE_CACHE        (1 << 4)
#define PG_PDE_4MB              (1 << 7)

#define KERNEL_BASE_VIRTUAL_ADDR 0xC0000000

void page_table_init();
void page_directory_init();
void enable_paging();
void paging_init();
void map_page(uintptr_t virtual_addr, uintptr_t physical_addr, uint32_t flags);
void unmap_page(uintptr_t virtual_addr);
void run_paging_tests();
#endif