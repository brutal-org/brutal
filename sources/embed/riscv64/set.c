#include <embed/arch.h>

void *arch_mem_set(void *d, uint8_t s, size_t c)
{
    uint8_t *dest = (uint8_t *)d;

    for (size_t i = 0; i < c; i++)
    {
        dest[i] = s;
    }

    return d;
}

