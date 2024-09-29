#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#include <string.h>
#include <kprintf.h>

#define BIT_MAP_ADDR 0x200000
#define BIT_MAP 32768 
#define PAGE_SIZE 4096

extern uint8_t* memory_bitmap;
extern size_t memory_bitmap_size;

//Functions:

void physical_memory_init(uint32_t mem_size);

void* alloc_physical_page();

void free_physical_page(void* page);

size_t get_free_page_count();
static int find_first_free_page();
int is_page_free(void* ptr);
#endif //PHYSICAL_MEMORY_H