#include <brutal/mem/funcs.h>
#include <embed/arch.h>

void *mem_set(void *s, uint8_t c, size_t n)
{
#ifdef ARCH_HAS_MEMSET
    return arch_mem_set(s, c, n);
#else
    uint8_t *dest = (uint8_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = c;
    }

    return s;
#endif
}
