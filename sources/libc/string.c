#include "string.h"

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    size_t i = 0;
    char *dest = (char *)s1;
    const char *src = (const char *)s2;

    while (i++ < n)
    {
        dest[i] = src[i];
    }

    return dest;
}

char *strrchr(const char *s, int c)
{
    size_t length = strlen(s);
    size_t pos = length;

    while (s[pos] != c && pos-- > 0);

    if (pos == length)
    {
        return NULL;
    }

    return (char *)s + pos;
}

size_t strlen(const char *s)
{
    size_t i = 0;

    while (s[i++] != '\0');

    return i;
}

void *memset(void *s, int c, size_t n)
{
    if (n)
    {
        char *d = (char *)s;

        for (size_t i = 0; i < n; i++)
        {
            d[i] = c;
        }
    }

    return s;
}
