#include <kernel/mm/paging.h>

// Page directory, covering 4GB of virtual memory (1024 entries * 4MB per entry)
uint32_t page_directory[PAGE_DIRECTORY_SIZE]__attribute__((aligned(PAGE_SIZE)));

// Page tables, one for each entry in the page directory
// Each page table covers 4MB of virtual memory (1024 entries * 4KB per entry)
uint32_t page_tables[PAGE_DIRECTORY_SIZE][PAGE_TABLE_SIZE]__attribute__((aligned(PAGE_SIZE)));

/**
 * @brief Maps a virtual page to a physical page.
 *
 * @param virtual_address The virtual address of the page to map.
 * @param physical_address The physical address of the page to map to.
 * @param flags The flags to set for the page table entry.
 */
void map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags)
{
    // Calculate the page directory index and page table index from the virtual address
    uint32_t page_dir_idx = (virtual_address >> 22) & 0x3FF; // High 10 bits
    uint32_t page_table_idx = (virtual_address >> 12) & 0x3FF; // Middle 10 bits

    // Get the page table entry from the page directory
    uint32_t* page_table = (uint32_t*)(page_directory[page_dir_idx] & ~0xFFF);

    // If the page table is not present, allocate a new page table
    if (page_table == NULL)
    {
        page_table = (uint32_t*)alloc_physical_page();
        memset(page_table, 0, PAGE_SIZE);

        // Set the page directory entry to point to the new page table
        page_directory[page_dir_idx] = (uint32_t)page_table | flags;
    }

    // Set the page table entry to map the virtual address to the physical address
    page_table[page_table_idx] = physical_address | flags;
}

/**
 * @brief Unmaps a virtual page.
 *
 * @param virtual_address The virtual address of the page to unmap.
 */
void unmap_page(uint32_t virtual_address)
{
    // Calculate the page directory index and page table index from the virtual address
    uint32_t page_dir_idx = (virtual_address >> 22) & 0x3FF; // High 10 bits
    uint32_t page_table_idx = (virtual_address >> 12) & 0x3FF; // Middle 10 bits

    // Get the page directory entry
    uint32_t* page_dir = (uint32_t*)page_directory;
    uint32_t page_dir_entry = page_dir[page_dir_idx];

    // If the page directory entry is not present, return
    if (!(page_dir_entry & 1))
    {
        return;
    }

    // Get the page table entry
    uint32_t* page_table = (uint32_t*)(page_dir_entry & ~0xFFF);
    uint32_t page_table_entry = page_table[page_table_idx];

    // If the page table entry is not present, return
    if (!(page_table_entry & 1))
    {
        return;
    }

    // Clear the page table entry
    page_table[page_table_idx] = 0;

    // Invalidate the TLB entry for the virtual address
    asm volatile("invlpg (%0)" : : "r"(virtual_address) : "memory");
}

/**
 * @brief Initializes the page tables.
 *
 * This function sets up the page tables to map the first 4GB of virtual memory
 * to the first 4GB of physical memory. This is a one-to-one mapping, meaning
 * that each virtual address corresponds to the same physical address.
 *
 * The function iterates through each entry in the page tables and sets the
 * following flags:
 *  - Present (bit 0): Set to 1, indicating that the page is present in memory.
 *  - Read/Write (bit 1): Set to 1, allowing both read and write access to the page.
 *  - User/Supervisor (bit 2): Set to 1, allowing access from both user and kernel mode.
 */
void page_table_init()
{
    // Iterate through each page directory entry
    for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++)
    {
        // Iterate through each page table entry within the current page directory entry
        for (int j = 0;j < PAGE_DIRECTORY_SIZE;j++)
        {
            // Calculate the physical address corresponding to this page table entry
            // The address is calculated as:
            //  - (i * PAGE_DIRECTORY_SIZE * PAGE_SIZE): Base address of the current page directory entry
            //  - (j * PAGE_SIZE): Offset within the current page directory entry
            // The result is then bitwise ORed with 3 to set the Present, Read/Write, and User/Supervisor flags.
            page_tables[i][j] = (((i * PAGE_DIRECTORY_SIZE * PAGE_SIZE) + (j * PAGE_SIZE)) | 3);
        }
    }
}

/**
 * @brief Initializes the page directory.
 *
 * This function sets up the page directory to point to the page tables.
 *
 * The function iterates through each entry in the page directory and sets the
 * following flags:
 *  - Present (bit 0): Set to 1, indicating that the page table is present in memory.
 *  - Read/Write (bit 1): Set to 1, allowing both read and write access to the page table.
 *  - User/Supervisor (bit 2): Set to 1, allowing access from both user and kernel mode.
 */
void page_directory_init()
{
    // Iterate through each page directory entry
    for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++)
    {
        // Set the page directory entry to point to the corresponding page table
        // The address of the page table is bitwise ORed with 3 to set the Present, Read/Write, and User/Supervisor flags.
        page_directory[i] = ((uint32_t)page_tables[i]) | 3;
    }
}

/**
 * @brief Enables paging.
 *
 * This function enables paging by loading the address of the page directory
 * into the CR3 register and setting the PG bit in the CR0 register.
 */
void enable_paging()
{
    // Load the address of the page directory into the CR3 register
    asm volatile("mov %0, %%cr3" : : "r"(page_directory));

    // Set the PG bit in the CR0 register to enable paging
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

/**
 * @brief Maps the kernel's higher half (3GB-4GB) to the first 4MB of physical memory.
 *
 * This function is necessary because the kernel is loaded at 0xC0000000 (3GB),
 * but it needs to access the lower 4MB of physical memory where the VGA buffer
 * and other important data structures are located.
 */
void map_kernel_high_half()
{
    // The 768th entry in the page directory corresponds to the virtual address range 3GB-3GB+4MB
    // We set this entry to point to the first page table, which maps the first 4MB of physical memory.
    page_directory[768] = ((uint32_t)page_tables[0]) | 3;
}

/**
 * @brief Initializes paging.
 *
 * This function initializes the page tables, page directory, and enables paging.
 */
void paging_init()
{
    page_table_init();
    page_directory_init();
    map_kernel_high_half();
    enable_paging();
}

void test_page_table_init()
{
    paging_init();  // Initialize paging system

    for (int i = 0; i < PAGE_DIRECTORY_SIZE; i++)
    {
        for (int j = 0; j < PAGE_TABLE_SIZE; j++)
        {
            uintptr_t expected_address = (i * PAGE_DIRECTORY_SIZE * PAGE_SIZE) + (j * PAGE_SIZE);
            if ((page_tables[i][j] & ~0xFFF) != expected_address)
            {
                kprintf("Error: Page table entry (%d, %d) points to wrong address 0x%x, expected 0x%x\n", i, j, page_tables[i][j], expected_address);
                return;
            }
        }
    }
    kprintf("Page table initialization test passed!\n");
}

void test_map_page()
{
    uintptr_t virtual_addr = 0x400000;  // Arbitrary virtual address
    uintptr_t physical_addr = 0x100000;  // Arbitrary physical address

    map_page(virtual_addr, physical_addr, 0x3);  // Map with read/write permissions

    uint32_t page_dir_idx = (virtual_addr >> 22) & 0x3FF;
    uint32_t page_table_idx = (virtual_addr >> 12) & 0x3FF;

    if ((page_tables[page_dir_idx][page_table_idx] & ~0xFFF) != physical_addr)
    {
        kprintf("Error: Virtual address 0x%x not mapped to correct physical address 0x%x\n", virtual_addr, physical_addr);
    }
    else
    {
        kprintf("Mapping test passed: 0x%x -> 0x%x\n", virtual_addr, physical_addr);
    }
}

void test_unmap_page()
{
    uintptr_t virtual_addr = 0x400000;  // Arbitrary virtual address

    unmap_page(virtual_addr);  // Unmap the page

    uint32_t page_dir_idx = (virtual_addr >> 22) & 0x3FF;
    uint32_t page_table_idx = (virtual_addr >> 12) & 0x3FF;

    if (page_tables[page_dir_idx][page_table_idx] != 0)
    {
        kprintf("Error: Virtual address 0x%x was not unmapped correctly\n", virtual_addr);
    }
    else
    {
        kprintf("Unmapping test passed: 0x%x\n", virtual_addr);
    }
}

void run_paging_tests()
{
    kprintf("Running paging tests...\n");

    test_page_table_init();
    test_map_page();
    test_unmap_page();

    kprintf("Paging tests complete.\n");
}

