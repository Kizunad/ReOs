#include <string.h>

void* memmove(void* dest, const void* src, size_t num)
{
    if (dest < src)
    {
        return memcpy(dest, src, num);
    }
    else
    {
        uint8_t* dest_ptr = (uint8_t*)dest + num - 1;
        const uint8_t* src_ptr = (const uint8_t*)src + num - 1;

        while (num--)
        {
            *dest_ptr-- = *src_ptr--;
        }

        return dest;
    }
}