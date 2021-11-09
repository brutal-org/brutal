#include <brutal/debug.h>
#include <brutal/ds/bitmap.h>
#include <brutal/mem.h>

void bitmap_init(Bitmap *self, void *data, size_t size)
{
    *self = (Bitmap){
        .data = (uint8_t *)data,
        .size = size,
    };
}

void bitmap_set_range(Bitmap *bitmap, USizeRange range, bool value)
{
    for (size_t i = 0; i < range.size; i++)
    {
        bitmap_set(bitmap, range.base + i, value);
    }
}

void bitmap_fill(Bitmap *bitmap, bool value)
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

USizeRange bitmap_find_free(Bitmap const *bitmap, size_t start, size_t size, bool upper)
{
    if (start == (size_t)-1)
    {
        start = bitmap_len(bitmap);
    }

    if (bitmap->size == 0)
    {
        return (USizeRange){};
    }

    size_t range_start = 0;
    size_t range_size = 0;

    for (size_t i = start;
         upper ? i > 0 : i < bitmap_len(bitmap);
         i += upper ? -1 : +1)
    {
        if (bitmap_get(bitmap, i) == 0)
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
            return (USizeRange){range_start, range_size};
        }
    }

    return (USizeRange){};
}
