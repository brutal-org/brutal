#include "misc.h"
#include <stdbool.h>

size_t u16strlen(const uint16_t *str)
{
    const uint16_t *pos = str;

    while (*pos++)
        ;
    return pos - str - 1;
}

int u16strcmp(const uint16_t *l, const uint16_t *r)
{
    while (*l == *r && *l != '\0')
    {
        ++l;
        ++r;
    }

    if (*l == *r)
        return 0;

    if (*l < *r)
        return -1;
    else
        return 1;
}

uint16_t *to_u16strncpy(uint16_t *dst, const char *src, size_t size)
{
    size_t i = 0;

    for (; i < size && src[i]; ++i)
        dst[i] = src[i];

    for (; i < size; ++i)
        dst[i] = '\0';

    return dst;
}
