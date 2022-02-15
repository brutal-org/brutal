#pragma once

#include <brutal/base/result.h>
#include <brutal/debug/assert.h>
#include <brutal/io/funcs.h>

typedef uint_fast64_t BitBuf;

/**
  @brief Number of bits the bitbuffer variable can hold.
*/
#define BITBUF_NBITS (8 * sizeof(BitBuf) - 1)

/**
  @brief The maximum number of bits that can be ensured in the bitbuffer variable,
*/
#define MAX_ENSURE (BITBUF_NBITS - 7)

typedef struct
{
    /* The output stream where we flush our bits into */
    IoReader reader;
    /* Bits that haven't yet been written to the output buffer */
    BitBuf bitbuf;
    /* Number of bits currently held in @bitbuf */
    size_t bitcount;
} IoRBits;

static inline void io_rbits_init(IoRBits *self, IoReader reader)
{
    self->reader = reader;
    self->bitbuf = 0;
    self->bitcount = 0;
}

/**
 @brief Check that our bitbuffer can hold atleast \p num_bits
*/
static inline bool io_rbits_can_ensure(const size_t num_bits)
{
    return num_bits <= MAX_ENSURE;
}

/**
 @brief Make sure we have atleast \p num_bits
*/
static inline IoResult io_rbits_ensure_bits(IoRBits *self, const size_t num_bits)
{
    assert_truth(io_rbits_can_ensure(num_bits));
    uint8_t b;
    while (self->bitcount < num_bits)
    {
        TRY(IoResult, io_read(self->reader, &b, 1));
        self->bitbuf |= b << self->bitcount;
        self->bitcount += 8;
    }
    return OK(IoResult, num_bits);
}

static inline IoResult io_rbits_get_byte(IoRBits *self)
{
    assert_lower_equal(self->bitcount, BITBUF_NBITS - 8);
    uint8_t b;
    TRY(IoResult, io_read(self->reader, &b, 1));
    self->bitbuf |= b << self->bitcount;
    self->bitcount += 8;
    return OK(IoResult, 8);
}

/**
  @brief Return the next \p num_bits from the bitbuffer variable without removing them.
*/
static inline uint32_t io_rbits_get_bits(IoRBits *self, const size_t num_bits)
{
    assert_lower_equal(num_bits, self->bitcount);
    return self->bitbuf & ((1 << num_bits) - 1);
}

/**
  @brief Remove the next \p num_bits from the bitbuffer variable.
*/
static inline void io_rbits_remove_bits(IoRBits *self, const size_t num_bits)
{
    assert_lower_equal(num_bits, self->bitcount);
    self->bitbuf >>= num_bits;
    self->bitcount -= num_bits;
}

/**
  @brief Return the next \p num_bits from the bitbuffer variable with removing them.
*/
static inline uint32_t io_rbits_pop_bits(IoRBits *self, const size_t num_bits)
{
    assert_lower_equal(num_bits, self->bitcount);
    uint32_t result = io_rbits_get_bits(self, num_bits);
    io_rbits_remove_bits(self, num_bits);
    return result;
}

/**
  @brief Flush to byte boundary
*/
static inline void io_rbits_align(IoRBits *self)
{
    size_t to_flush = self->bitcount % 8;
    io_rbits_remove_bits(self, to_flush);
}
