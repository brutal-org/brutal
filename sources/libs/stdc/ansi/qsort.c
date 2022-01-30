#define _BRUTAL_SOURCE

#include <stdlib.h>
#include <string.h>

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
