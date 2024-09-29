#ifndef STRING_H
#define STRING_H
#include <stddef.h>
#include <stdint.h>

void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
void* memmove(void*, const void*, size_t);
int memcmp(const void*, const void*, size_t);

size_t strlen(const char*);
char* strcpy(char*, const char*);


#endif //STRING_H