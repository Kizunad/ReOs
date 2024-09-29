#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/mm/physical_memory.h>
#include <kernel/mm/paging.h>

#define HEAP_START 0xA0000000
#define HEAP_INIT_SIZE 0x100000
#define HEAP_MIN_SIZE 0x70000

void heap_init();
void* kmalloc(size_t size);
void kfree(void* ptr);
void* kmalloc_a(size_t size);

#endif