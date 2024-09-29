#include <kernel/mm/physical_memory.h>
#include <unit_tests/test_phymem.h>
// 定义最大内存大小为 256MB
#define MAX_MEMORY_SIZE (256 * 1024 * 1024)

// 内存位图，用于标记每个页面是否空闲
uint8_t* memory_bitmap;
// 总内存大小（以字节为单位）
size_t total_memory_size = 0;
// 总页面数
size_t total_pages = 0;
// 空闲页面数
size_t free_pages = 0;

/**
 * @brief 初始化物理内存管理器。
 *
 * @param mem_size 内存大小（以字节为单位）。
 */
void physical_memory_init(uint32_t mem_size)
{
    // 如果内存大小超过最大限制，则设置为最大限制
    if (mem_size > MAX_MEMORY_SIZE)
    {
        mem_size = MAX_MEMORY_SIZE;
    }

    // 初始化全局变量
    total_memory_size = mem_size;
    total_pages = total_memory_size / PAGE_SIZE;
    memory_bitmap = (uint8_t*)BIT_MAP_ADDR;

    // 将内存位图清零，表示所有页面都空闲
    memset(memory_bitmap, 0, BIT_MAP);
    free_pages = total_pages;

    // 查找第一个空闲页面
    int first_free_page = find_first_free_page();

    // 打印初始化信息
    kprintf("Physical Memory Initialized with total pages of %d, the first free page is at %d, bitmap %d\n", free_pages, first_free_page, memory_bitmap);

    verify_physical_memory();
}

/**
 * @brief 查找第一个空闲页面。
 *
 * @return 第一个空闲页面的索引，如果找不到则返回 -1。
 */
static int find_first_free_page()
{
    // 遍历内存位图的每个字节
    for (size_t i = 0; i < total_pages / 8; i++)
    {
        // 如果该字节不全是 1，说明该字节中存在空闲页面
        if (memory_bitmap[i] != 0xff)
        {
            // 遍历该字节的每个位
            for (size_t j = 0; j < 8; j++)
            {
                // 如果该位为 0，说明该页面空闲
                if ((memory_bitmap[i] & (1 << j)) == 0)
                {
                    // 返回该页面的索引
                    return i * 8 + j;
                }
            }
        }
    }

    // 找不到空闲页面，返回 -1
    return -1;
}

/**
 * @brief 将指定页面标记为已使用。
 *
 * @param page_index 页面索引。
 */
static void mark_page_as_used(size_t page_index)
{
    // 计算该页面在内存位图中的字节索引和位索引
    size_t byte_index = page_index / 8;
    size_t bit_index = page_index % 8;

    // 将该页面对应的位设置为 1，表示已使用
    *(memory_bitmap + byte_index) |= (1 << bit_index);
}

/**
 * @brief 将指定页面标记为空闲。
 *
 * @param page_index 页面索引。
 */
static void mark_page_as_free(size_t page_index)
{
    // 计算该页面在内存位图中的字节索引和位索引
    size_t byte_idx = page_index / 8;
    size_t bit_idx = page_index % 8;

    // 将该页面对应的位设置为 0，表示空闲
    memory_bitmap[byte_idx] &= ~(1 << bit_idx);
}

/**
 * @brief 分配一个物理页面。
 *
 * @return 分配的物理页面的地址，如果分配失败则返回 NULL。
 */
void* alloc_physical_page()
{
    // 查找第一个空闲页面
    int page_idx = find_first_free_page();
    if (page_idx == -1)
    {
        // 没有空闲页面，打印错误信息并返回 NULL
        kprintf("Out of memory!\n");
        return NULL;
    }

    // 将该页面标记为已使用
    mark_page_as_used(page_idx);
    // 减少空闲页面计数
    free_pages--;

    // 计算分配的物理地址
    uintptr_t alloc_address = (page_idx * PAGE_SIZE);
    // 如果分配的地址在内核和位图所在的区域（前 2MB + BITMAP），则继续查找下一个空闲页面
    if (alloc_address < 0x208000)
    {
        return alloc_physical_page();
    }

    // 打印调试信息
    kprintf("Allocated page %d at address %x\n", page_idx, (void*)(page_idx * PAGE_SIZE));

    // 返回分配的物理地址
    return (void*)(page_idx * PAGE_SIZE);
}

/**
 * @brief 释放一个物理页面。
 *
 * @param ptr 物理页面的地址。
 */
void free_physical_page(void* ptr)
{
    // 计算该页面的索引
    size_t page_idx = (size_t)ptr / PAGE_SIZE;

    // 将该页面标记为空闲
    mark_page_as_free(page_idx);

    // 增加空闲页面计数
    free_pages++;
}

/**
 * @brief 检查指定页面是否空闲。
 *
 * @param ptr 物理页面的地址。
 *
 * @return 如果该页面空闲则返回 1，否则返回 0。
 */
int is_page_free(void* ptr)
{
    // 计算该页面的索引
    size_t page_idx = (size_t)ptr / PAGE_SIZE;

    // 计算该页面在内存位图中的字节索引和位索引
    size_t byte_idx = page_idx / 8;
    size_t bit_idx = page_idx % 8;

    // 检查该页面对应的位是否为 0
    if (memory_bitmap[byte_idx] & (1 << bit_idx))
    {
        // 不为空闲
        return 0;
    }
    else
    {
        // 空闲
        return 1;
    }
}

/**
 * @brief 获取空闲页面数量。
 *
 * @return 空闲页面数量。
 */
size_t get_free_page_count()
{
    return free_pages;
}
