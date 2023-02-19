#define _BRUTAL_SOURCE
#include <string.h>

size_t strlcpy(char *dst, char const *src, size_t size)
{
    size_t len = strlen(src);

    if (size)
    {
        size_t len2 = len;

        if (len2 >= size)
        {
            len2 = size - 1;
        }

        memcpy(dst, src, len2);
        dst[len2] = '\0';
    }

    return len;
}

size_t strlcat(char *dst, char const *src, size_t size)
{
    size_t len = strlen(dst);

    if (size <= len)
    {
        return len + strlen(src);
    }

    size_t len2 = len;

    if (len2 >= size - 1)
    {
        len2 = size - 2;
    }

    memcpy(dst + len2, src, size - len2 - 1);
    dst[size - 1] = '\0';

    return len + strlen(src);
}

void *memfrob(void *s, size_t n)
{
    unsigned char *p = s;

    while (n--)
    {
        *p++ ^= 0x42;
    }

    return s;
}

void memswap(void *block1, void *block2, size_t n)
{
    char *p1 = block1;
    char *p2 = block2;
    while (n--)
    {
        char c = *p1;
        *p1++ = *p2;
        *p2++ = c;
    }
}
