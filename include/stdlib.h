#ifndef __STDLIB_H
#define __STDLIB_H

#include <types.h>
#include <limits.h>
#include <vsprintf.h>


void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
void *memsetl(void *addr, unsigned long c, size_t count);
void *memsetw(void *addr, unsigned short c, size_t count);
void *memset(void *addr, int c, size_t count);
int strcmp(char *str1, char *str2);
void strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
size_t strlen(const char* str);
size_t strnlen(const char *s, unsigned int count);


#endif /* __STDLIB_H */
