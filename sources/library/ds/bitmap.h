#pragma once

#include <library/base/std.h>

#define BITMAP_BYTE_INDEX(index) ((index) / 8)
#define BITMAP_BIT_INDEX(index) ((index) % 8)

struct bitmap
{
    uint8_t *data;
    size_t size;
};

struct bitmap bitmap(void *data, size_t size);

void bitmap_fill(struct bitmap *bitmap, bool value);

static inline bool bitmap_get(struct bitmap const *bitmap, size_t index)
{
    size_t const byte_index = BITMAP_BYTE_INDEX(index);
    size_t const bit_index = BITMAP_BIT_INDEX(index);

    return bitmap->data[byte_index] & (1 << bit_index);
}

static inline void bitmap_set(struct bitmap *bitmap, size_t index, bool value)
{
    size_t const byte = BITMAP_BYTE_INDEX(index);
    size_t const bit_index = BITMAP_BIT_INDEX(index);

    if (value)
    {
        bitmap->data[byte] |= (1 << (bit_index));
    }
    else
    {
        bitmap->data[byte] &= ~(1 << (bit_index));
    }
}

static inline void bitmap_set_range(struct bitmap *bitmap, size_t index, size_t count, bool value)
{
    for (size_t i = index; i < index + count; i++)
    {
        bitmap_set(bitmap, i, value);
    }
}
