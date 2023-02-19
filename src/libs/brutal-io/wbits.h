#pragma once

#include "bitbuf.h"
#include "funcs.h"

typedef struct
{
    /* The output stream where we flush our bits into */
    IoWriter writer;

    /* Bits that haven't yet been written to the output buffer */
    BitBuf bitbuf;

    /* Number of bits currently held in @bitbuf */
    size_t bitcount;
} IoWBits;

static inline void io_wbits_init(IoWBits *self, IoWriter writer)
{
    self->writer = writer;
    self->bitbuf = 0;
    self->bitcount = 0;
}

/**
 @brief Add some bits to the bitbuffer variable of the output bitstream.  The caller
        must make sure there is enough room.
*/
static inline void io_wbits_add_bits(IoWBits *self, const BitBuf bits, const size_t num_bits)
{
    self->bitbuf |= bits << self->bitcount;
    self->bitcount += num_bits;
}

/**
  @brief Flush bits that fit into bytes
*/
static inline void io_wbits_flush_bits(IoWBits *self)
{
    /* Flush a byte at a time. */
    while (self->bitcount >= 8)
    {
        io_write(self->writer, (uint8_t *)&self->bitbuf, 1);
        self->bitcount -= 8;
        self->bitbuf >>= 8;
    }
}

/**
  @brief Flush all remaining bits even if does not align to a byte boundary
*/
static inline void io_wbits_flush_bits_remaining(IoWBits *self)
{
    io_wbits_flush_bits(self);
    io_write(self->writer, (uint8_t *)&self->bitbuf, 1);
    self->bitcount = 0;
    self->bitbuf = 0;
}

/**
  @brief Align the bitstream on a byte boundary.
*/
static inline void io_wbits_align_bitstream(IoWBits *self)
{
    self->bitcount += -self->bitcount & 7;
    io_wbits_flush_bits(self);
}
