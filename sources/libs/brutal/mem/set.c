#include <brutal/mem.h>

#if defined(__x86_64__) || defined(__x86_32__)

void __stosb_set(void *d, uint8_t s, size_t c)
{
    asm volatile(
        "rep stosb"
        : "=D"(d), "=c"(c)
        : "0"(d), "a"(s), "1"(c)
        : "memory");
}

#endif

void *mem_set(void *s, uint8_t c, size_t n)
{

#if defined(__x86_64__) || defined(__x86_32__)

    __stosb_set(s, c, n);
#else
    uint8_t *dest = (uint8_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = c;
    }
#endif
    return s;
}
