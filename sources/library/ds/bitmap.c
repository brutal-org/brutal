#include <library/ds/bitmap.h>
#include <library/mem.h>

void bitmap_init(struct bitmap *self, void *data, size_t size)
{
    *self = (struct bitmap){
        .data = data,
        .size = size,
    };
}

void bitmap_set_range(struct bitmap *bitmap, bitmap_range_t range, bool value)
{
    for (size_t i = 0; i < range.size; i++)
    {
        bitmap_set(bitmap, range.base + i, value);
    }
}

void bitmap_fill(struct bitmap *bitmap, bool value)
{
    if (value)
    {
        mem_set(bitmap->data, 0xff, bitmap->size);
    }
    else
    {
        mem_set(bitmap->data, 0, bitmap->size);
    }
}

bitmap_range_t bitmap_find_range(struct bitmap const *bitmap, size_t start, size_t size, bool value)
{
    size_t range_start = 0;
    size_t range_size = 0;

    for (size_t i = start; i < bitmap->size * 8; i++)
    {
        if (bitmap_get(bitmap, i) == value)
        {
            if (range_size == 0)
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

        if (size <= range_size)
        {
            return (bitmap_range_t){range_start, range_size};
        }
    }

    return (bitmap_range_t){};
}