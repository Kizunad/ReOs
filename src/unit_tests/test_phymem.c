#include <unit_tests/test_phymem.h>

void verify_physical_memory()
{
    void* page_ptr = alloc_physical_page();
    if (is_page_free(page_ptr))
    {
        kprintf("Error: Allocated page is still marked as free!\n");
    }
    free_physical_page(page_ptr);
    if (!is_page_free(page_ptr))
    {
        kprintf("Error: Freed page is not marked as free!\n");
    }
}

void test_physical_memory_limits()
{
    for (size_t i = 0; i < get_free_page_count(); i++)
    {
        if (i > 0 && (alloc_physical_page() == NULL))
        {
            kprintf("Out of memory at page %d\n", i);
            return;
        }
    }
}