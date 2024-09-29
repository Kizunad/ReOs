#include <string.h>

char* strcpy(char* dest, const char* src)
{
    char buf;
    unsigned int i = 0;
    while ((buf = src[i]))
    {
        dest[i] = buf;
        i++;
    }
    dest[i] = '\0';
    return dest;
}