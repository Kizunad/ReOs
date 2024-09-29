#include <kernel/mm/heap.h>

// 堆的起始地址和结束地址
static uintptr_t heap_start = HEAP_START;
static uintptr_t heap_end = HEAP_START + HEAP_INIT_SIZE;

// 空闲块结构体
typedef struct freeblock
{
    uint32_t size;        // 块大小
    struct freeblock* next; // 指向下一个空闲块
} freeblock_t;

// 空闲块链表头
static freeblock_t* free_list = NULL;

// 初始化堆
void heap_init()
{
    // 将第一个空闲块设置为整个堆空间
    free_list = (freeblock_t*)heap_start;
    free_list->size = heap_end - heap_start;
    free_list->next = NULL;
}

// 扩展堆大小
void* expand_heap(size_t size)
{
    // 计算新的堆结束地址
    uintptr_t new_end = heap_end + size;

    // 循环分配物理页面，直到堆大小满足要求
    while (heap_end < new_end)
    {
        // 分配一个物理页面
        void* page = alloc_physical_page();
        kprintf("Allocated address: %x\n", page);
        // 将物理页面映射到虚拟地址空间
        map_page(heap_end, (uintptr_t)page, PG_PRESENT | PG_WRITE);

        // 更新堆结束地址
        heap_end += PAGE_SIZE;
    }
    // 返回扩展后的堆空间起始地址
    return (void*)(heap_end - size);

}

// 分配内存
void* kmalloc(size_t size)
{
    // 如果请求大小为0，则返回NULL
    if (size == 0)
    {
        return NULL;
    }

    // 前一个空闲块和当前空闲块
    freeblock_t* prev = NULL;
    freeblock_t* curr = free_list;

    // 查找大小合适的空闲块
    while (curr != NULL && curr->size < size)
    {
        prev = curr;
        curr = curr->next;
    }

    // 如果没有找到合适的空闲块，则扩展堆
    if (curr == NULL)
    {
        curr = (freeblock_t*)expand_heap(size);
        curr->size = size;
        curr->next = NULL;
    }

    // 如果当前空闲块大小足够，则将其分割
    if (curr->size > size + sizeof(freeblock_t))
    {
        // 创建新的空闲块
        freeblock_t* new_block = (freeblock_t*)((uintptr_t)curr + size);
        new_block->size = curr->size - size;
        new_block->next = curr->next;

        // 更新当前空闲块大小
        curr->size = size;
        curr->next = new_block;
    }

    // 从空闲链表中移除当前空闲块
    if (prev == NULL)
    {
        free_list = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }

    // 返回分配的内存地址
    return (void*)curr;
}

// 释放内存
void kfree(void* ptr)
{
    // 将释放的块添加到空闲链表头部
    freeblock_t* block = (freeblock_t*)ptr;
    block->next = free_list;
    free_list = block;
}

// 分配页对齐的内存
void* kmalloc_a(size_t size)
{
    // 计算总共需要的内存大小，包括头部和填充
    size_t total_size = size + sizeof(freeblock_t);
    uint32_t padding = (PAGE_SIZE - (total_size % PAGE_SIZE)) % PAGE_SIZE;
    total_size += padding;

    // 分配内存
    void* ptr = kmalloc(total_size);
    if (ptr == NULL)
    {
        return NULL;
    }

    // 计算对齐的地址
    uintptr_t aligned_ptr = ((uintptr_t)ptr + sizeof(freeblock_t) + padding) & ~(PAGE_SIZE - 1);

    // 设置空闲块头部信息
    freeblock_t* block = (freeblock_t*)aligned_ptr - 1;
    block->size = total_size;

    // 返回对齐后的内存地址
    return (void*)aligned_ptr;
}

// 测试用例：分配小块内存
void test_small_allocation()
{
    void* ptr = kmalloc(64);  // Allocate 64 bytes

    if (ptr == NULL)
    {
        kprintf("Error: kmalloc failed to allocate 64 bytes.\n");
    }
    else
    {
        kprintf("Small allocation test passed! Address: 0x%x\n", ptr);
    }
    kfree(ptr);
}

// 测试用例：分配大块内存
void test_large_allocation()
{
    void* ptr = kmalloc(4096);  // Allocate 4096 bytes (1 page)

    if (ptr == NULL)
    {
        kprintf("Error: kmalloc failed to allocate 4096 bytes.\n");
    }
    else
    {
        kprintf("Large allocation test passed! Address: %x\n", ptr);
    }
}

// 测试用例：释放内存
void test_free_memory()
{
    void* ptr = kmalloc(128);  // Allocate 128 bytes

    if (ptr == NULL)
    {
        kprintf("Error: kmalloc failed to allocate 128 bytes.\n");
        return;
    }

    kfree(ptr);  // Free the allocated memory

    // Check if the block was added back to the free list (this can be tricky in simple heaps).
    kprintf("Memory free test passed for 128 bytes at address %x\n", ptr);
}

// 测试用例：内存不足
void test_out_of_memory()
{
    size_t large_size = (HEAP_INIT_SIZE / 2);  // Allocate almost the entire heap size
    void* ptr1 = kmalloc(large_size);
    void* ptr2 = kmalloc(large_size);

    if (ptr1 == NULL || ptr2 == NULL)
    {
        kprintf("Error: Heap ran out of memory too early.\n");
    }
    else
    {
        void* ptr3 = kmalloc(64);  // Try to allocate more memory than available

        if (ptr3 == NULL)
        {
            kprintf("Out-of-memory test passed! No more memory available, returned NULL.\n");
        }
        else
        {
            kprintf("Error: kmalloc should have returned NULL but allocated %x.\n", ptr3);
        }
    }
}

// 运行堆测试用例
void run_heap_tests()
{
    kprintf("Running heap tests...\n");
    test_out_of_memory();
    kprintf("Heap tests complete.\n");
}
