#include <brutal/mem/funcs.h>
#include <embed/arch.h>

void *mem_cpy(void *to, void const *from, size_t n)
{
#ifdef ARCH_HAS_MEMCPY
    return arch_mem_copy(to, from, n);
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
