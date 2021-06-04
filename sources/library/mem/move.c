#include <library/mem.h>

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
