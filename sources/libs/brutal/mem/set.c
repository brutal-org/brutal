#include <brutal/mem.h>

void *mem_set(void *s, uint8_t c, size_t n)
{
    uint8_t *dest = (uint8_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = c;
    }

    return s;
}
