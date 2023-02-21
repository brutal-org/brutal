#define _BRUTAL_SOURCE

#include <stdlib.h>
#include <string.h>
void memswap(void *block1, void *block2, size_t n)
{
    char *p1 = block1;
    char *p2 = block2;
    while (n--)
    {
        char c = *p1;
        *p1++ = *p2;
        *p2++ = c;
    }
}

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(void const *, void const *))
{
    // FIXME: Write real quick sort

    for (size_t i = 0; i + 1 < nmemb; i++)
    {
        for (size_t j = i + 1; j < nmemb; j++)
        {
            void *a = base + (i * size);
            void *b = base + (j * size);

            if (compar(a, b) > 0)
            {
                memswap(a, b, size);
            }
        }
    }
}
