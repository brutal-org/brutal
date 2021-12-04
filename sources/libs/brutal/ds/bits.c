#include <brutal/debug.h>
#include <brutal/ds/bits.h>
#include <brutal/mem.h>

void bits_init(Bits *self, void *data, size_t size)
{
    *self = (Bits){
        .data = (uint8_t *)data,
        .size = size,
    };
}

void bits_set_range(Bits *bits, BitsRange range, bool value)
{
    for (size_t i = 0; i < range.size; i++)
    {
        bits_set(bits, range.base + i, value);
    }
}

void bits_fill(Bits *bits, bool value)
{
    if (value)
    {
        mem_set(bits->data, 0xff, bits->size);
    }
    else
    {
        mem_set(bits->data, 0, bits->size);
    }
}

BitsRange bits_find_free(Bits const *bits, size_t start, size_t size, bool upper)
{
    if (start == (size_t)-1)
    {
        start = bits_len(bits);
    }

    if (bits->size == 0)
    {
        return (BitsRange){};
    }

    size_t range_start = 0;
    size_t range_size = 0;

    for (size_t i = start;
         upper ? i > 0 : i < bits_len(bits);
         i += upper ? -1 : +1)
    {
        if (bits_get(bits, i) == 0)
        {
            if (range_size == 0 || upper)
            {
                range_start = i;
            }

            range_size++;
        }
        else
        {
            range_size = 0;
            range_start = 0;
        }

        if (size == range_size)
        {
            return (BitsRange){range_start, range_size};
        }
    }

    return (BitsRange){};
}
