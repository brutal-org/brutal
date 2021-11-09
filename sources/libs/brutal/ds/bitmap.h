#pragma once

#include <brutal/base/range.h>
#include <brutal/base/std.h>

#define BITMAP_BYTE_INDEX(index) ((index) / 8)
#define BITMAP_BIT_INDEX(index) ((index) % 8)

typedef struct
{
    uint8_t *data;
    size_t size;
} Bitmap;

static inline bool bitmap_get(Bitmap const *bitmap, size_t index)
{
    size_t const byte_index = BITMAP_BYTE_INDEX(index);
    size_t const bit_index = BITMAP_BIT_INDEX(index);

    return bitmap->data[byte_index] & (1 << bit_index);
}

static inline void bitmap_set(Bitmap *bitmap, size_t index, bool value)
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

void bitmap_init(Bitmap *self, void *data, size_t size);

void bitmap_set_range(Bitmap *bitmap, USizeRange range, bool value);

static inline size_t bitmap_len(const Bitmap *bitmap)
{
    return bitmap->size * 8;
}

void bitmap_fill(Bitmap *bitmap, bool value);

USizeRange bitmap_find_free(Bitmap const *bitmap, size_t start, size_t size, bool upper);
