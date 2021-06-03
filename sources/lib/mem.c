#include <mem.h>

#if defined(__x86_64__) || defined(__x86_32__)

static inline void __movsb_copy(void *from, const void *to, size_t size)
{
    asm volatile("rep movsb"
                 : "=D"(from),
                   "=S"(to),
                   "=c"(size)
                 : "D"(from),
                   "S"(to),
                   "c"(size)
                 : "memory");
}

#endif

void *mem_cpy(void *s1, const void *s2, size_t n)
{
#if defined(__x86_64__) || defined(__x86_32__)
    __movsb_copy(s1, s2, n);
#else
    uint8_t *dest = s1;
    uint8_t *src = s2;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
#endif

    return s1;
}

void *mem_set(void *s, uint8_t c, size_t n)
{
    uint8_t *dest = (uint8_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = c;
    }

    return s;
}

void *mem_move(void *dest, const void *src, size_t n)
{
    const unsigned char *usrc = (const unsigned char *)src;
    unsigned char *udest = (unsigned char *)dest;

    if (udest < usrc)
    {
        for (size_t i = 0; i < n; i++)
        {
            udest[i] = usrc[i];
        }
    }
    else if (udest > usrc)
    {
        for (size_t i = n; i > 0; i--)
        {
            udest[i - 1] = usrc[i - 1];
        }
    }

    return dest;
}
