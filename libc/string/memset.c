#include <string.h>
// Duff's device solution, https://en.wikipedia.org/wiki/Duff%27s_device
void* memset(void* ptr, int value, size_t num)
{
    uint8_t* dest_ptr = (uint8_t*)ptr;

    for (size_t i = 0; i < num; i++)
    {
        *(dest_ptr + i) = (uint8_t)value;
    }

    return ptr;
}

