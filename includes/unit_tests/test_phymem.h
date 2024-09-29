#ifndef TEST_PHYMEM_H
#define TEST_PHYMEM_H

#include <kernel/mm/physical_memory.h>
#include <kprintf.h>

void verify_physical_memory();
void test_physical_memory_limits();

#endif