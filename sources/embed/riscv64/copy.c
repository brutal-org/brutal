#include <embed/arch.h>

void *arch_mem_copy(void *to, void const *from, size_t size)
{

    uint8_t *dest = (uint8_t *)to;
    uint8_t const *src = (uint8_t const *)from;

    for (size_t i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }

    return to;
}

