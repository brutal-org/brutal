#include <library/ds/bitmap.h>
#include <library/mem.h>

struct bitmap bitmap(void *data, size_t size)
{
    return (struct bitmap){
        .data = data,
        .size = size,
    };
}

void bitmap_fill(struct bitmap *bitmap, bool value)
{
    if (value)
    {
        mem_set(bitmap->data, 0xff, bitmap->size / 8);
    }
    else
    {
        mem_set(bitmap->data, 0, bitmap->size / 8);
    }
}
