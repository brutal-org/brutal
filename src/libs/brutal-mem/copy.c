#include <embed/mem.h>

#include "funcs.h"

void *mem_cpy(void *to, void const *from, size_t n)
{
#ifdef EMBED_HAS_MEMCPY
    return embed_mem_copy(to, from, n);
#else
    uint8_t *dest = (uint8_t *)to;
    uint8_t const *src = (uint8_t const *)from;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }

    return to;
#endif
}
