#pragma once

#include <brutal-base>

#define BITS_BYTE_INDEX(index) ((index) / 8)
#define BITS_BIT_INDEX(index) ((index) % 8)

typedef struct
{
    uint8_t *data;
    size_t size;
} Bits;

typedef Range(size_t) BitsRange;

static inline bool bits_get(Bits const *bits, size_t index)
{
    size_t const byte_index = BITS_BYTE_INDEX(index);
    size_t const bit_index = BITS_BIT_INDEX(index);

    return bits->data[byte_index] & (1 << bit_index);
}

static inline void bits_set(Bits *bits, size_t index, bool value)
{
    size_t const byte_index = BITS_BYTE_INDEX(index);
    size_t const bit_index = BITS_BIT_INDEX(index);

    if (value)
    {
        bits->data[byte_index] |= (1 << (bit_index));
    }
    else
    {
        bits->data[byte_index] &= ~(1 << (bit_index));
    }
}

void bits_init(Bits *self, void *data, size_t size);

size_t bits_set_range(Bits *bits, BitsRange range, bool value);

static inline size_t bits_len(Bits const *bits)
{
    return bits->size * 8;
}

void bits_fill(Bits *bits, bool value);

BitsRange bits_find_free(Bits const *bits, size_t start, size_t size, bool upper);
