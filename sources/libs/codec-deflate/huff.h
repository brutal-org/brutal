#pragma once

#include <brutal-io/rbits.h>

#include "errors.h"

typedef struct
{
    uint16_t counts[16];   // Number of codes with a given length
    uint16_t symbols[288]; // Symbols sorted by code
    int32_t max_sym;
} HuffTree;

static inline MaybeError build_huff_tree(HuffTree *t, const uint8_t *lengths, uint32_t num)
{
    uint16_t offs[16];
    size_t i, num_codes, available;

    assert_lower_equal(num, 288u);

    for (i = 0; i < 16; ++i)
    {
        t->counts[i] = 0;
    }

    t->max_sym = -1;

    // Count number of codes for each non-zero length
    for (i = 0; i < num; ++i)
    {
        assert_lower_than(lengths[i], 16u);

        if (lengths[i])
        {
            t->max_sym = i;
            t->counts[lengths[i]]++;
        }
    }

    // Compute offset table for distribution sort
    for (available = 1, num_codes = 0, i = 0; i < 16; ++i)
    {
        uint32_t used = t->counts[i];

        // Check length contains no more codes than available
        if (used > available)
        {
            return ERROR(ERR_NOT_ENOUGH_TABLE_SPACE);
        }
        available = 2 * (available - used);

        offs[i] = num_codes;
        num_codes += used;
    }

    // Check all codes were used, or for the special case of only one
    // code that it has length 1
    if ((num_codes > 1 && available > 0) ||
        (num_codes == 1 && t->counts[1] != 1))
    {
        return ERROR(ERR_INVALID_CODE);
    }

    // Fill in symbols sorted by code
    for (i = 0; i < num; ++i)
    {
        if (lengths[i])
        {
            t->symbols[offs[lengths[i]]++] = i;
        }
    }

    // For the special case of only one code (which will be 0) add a
    // code 1 which results in a symbol that is too large
    if (num_codes == 1)
    {
        t->counts[1] = 2;
        t->symbols[1] = t->max_sym + 1;
    }

    return SUCCESS;
}

typedef struct
{
    IoRBits *bit_reader;
    HuffTree *tree;
} HuffDecoder;

static inline void huff_decoder_init(HuffDecoder *dec, IoRBits *bit_reader, HuffTree *tree)
{
    dec->bit_reader = bit_reader;
    dec->tree = tree;
}

static inline uint16_t huff_decoder_next(HuffDecoder *dec)
{
    int32_t base = 0, offs = 0;

    // Get more bits while code index is above number of codes
    //
    // Rather than the actual code, we are computing the position of the
    // code in the sorted order of codes, which is the index of the
    // corresponding symbol.
    //
    // Conceptually, for each code length (level in the tree), there are
    // counts[len] leaves on the left and internal nodes on the right.
    // The index we have decoded so far is base + offs, and if that
    // falls within the leaves we are done. Otherwise we adjust the range
    // of offs and add one more bit to it.
    for (int32_t len = 1;; ++len)
    {
        io_rbits_ensure_bits(dec->bit_reader, 1);
        offs = 2 * offs + io_rbits_pop_bits(dec->bit_reader, 1);

        assert_lower_equal(len, 15);

        if (offs < dec->tree->counts[len])
        {
            break;
        }

        base += dec->tree->counts[len];
        offs -= dec->tree->counts[len];
    }

    assert_greater_equal(base + offs, 0);
    assert_lower_than(base + offs, 288);

    return dec->tree->symbols[base + offs];
}
