#include <brutal-debug>
#include <brutal-mem>

#include "bits.h"

void bits_init(Bits *self, void *data, size_t size)
{
    *self = (Bits){
        .data = (uint8_t *)data,
        .size = size,
    };
}

size_t bits_set_range(Bits *bits, BitsRange range, bool value)
{
    assert_lower_equal(range_end(range), bits_len(bits));

    size_t count = 0;

    for (size_t i = 0; i < range.size; i++)
    {
        if (bits_get(bits, range.base + i) != value)
        {
            bits_set(bits, range.base + i, value);
            count++;
        }
    }

    return count;
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
    assert_greater_than(bits_len(bits), (size_t)1);
    start = m_min(start, bits_len(bits));

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
