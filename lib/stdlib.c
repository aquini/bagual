#include "types.h"

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
	const uint8_t *sp = (const uint8_t *)src;
	uint8_t *dp = (uint8_t *)dest;
	for (; len != 0; len--)
		*dp++ = *sp++;
}

void *memsetl(void *addr, unsigned long c, size_t count)
{
        unsigned long *tmp = (unsigned long *)addr;
        while (count--)
                *tmp++ = c;

        return addr;
}

void *memsetw(void *addr, unsigned short c, size_t count)
{
        unsigned short *tmp = (unsigned short *)addr;
        while (count--)
                *tmp++ = c;

        return addr;
}

void *memset(void *addr, int c, size_t count)
{
        unsigned char *tmp = (unsigned char *)addr;
        while (count--)
                *tmp++ = c;

        return addr;
}

int strcmp(char *str1, char *str2)
{
	size_t i = 0;
	int failed = 0;
	while (str1[i] != '\0' && str2[i] != '\0') {
		if (str1[i] != str2[i]) {
			failed = 1;
			break;
		}
		i++;
	}

	if ((str1[i] == '\0' && str2[i] != '\0') ||
	    (str1[i] != '\0' && str2[i] == '\0'))
		failed = 1;

      return failed;
}

void strcpy(char *dest, const char *src)
{
	do {
		*dest++ = *src++;
	} while (*src != 0);
}

char *strcat(char *dest, const char *src)
{
	size_t i,j;
	for (i = 0; dest[i] != '\0'; i++)
		;
	for (j = 0; src[j] != '\0'; j++)
		dest[i+j] = src[j];
	dest[i+j] = '\0';
	return dest;
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while (str[ret] != 0)
		ret++;
	return ret;
}

size_t strnlen(const char *s, unsigned int count)
{
	const char *sc;
	if (!s)
		 return( 0 );

	for (sc = s; ( count-- ) && ( *sc != '\0' ); ++sc);

	return( sc - s );
}

