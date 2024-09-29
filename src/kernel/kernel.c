#include <kernel/kernel.h>



void kernel_main(multiboot_info_t* mbi)
{

    tty_init();

    physical_memory_init(mbi->mem_upper * 1024);

    paging_init();
    kprintf("paging init.\n");
    run_heap_tests();
    /*
    heap_init();
    kprintf("heap init.\n");
*/
}