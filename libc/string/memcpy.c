#include <string.h>

void* memcpy(void* dest, const void* src, size_t num)
{
    uint8_t* dest_ptr = (uint8_t*)dest;
    const uint8_t* src_ptr = (const uint8_t*)src;

    for (size_t i = 0; i < num; i++)
    {
        dest_ptr[i] = src_ptr[i];
        // Is equal to *(dest_ptr + i) = *(src_ptr + i);
    }

    return dest;
}
