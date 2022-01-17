#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/io/write.h>

typedef uint_fast64_t BitBuf;
#define BITBUF_NBITS (8 * sizeof(BitBuf))

typedef struct
{
    /* The output stream where we flush our bits into */
    IoWriter writer;
    /* Bits that haven't yet been written to the output buffer */
    BitBuf bitbuf;
    /* Number of bits currently held in @bitbuf */
    unsigned bitcount;
} IoBitWriter;

static inline void io_bw_init(IoBitWriter *self, IoWriter writer)
{
    self->writer = writer;
    self->bitbuf = 0;
    self->bitcount = 0;
}

/**
 @brief Add some bits to the bitbuffer variable of the output bitstream.  The caller
        must make sure there is enough room.
*/
static inline void
io_bw_add_bits(IoBitWriter *self,
               const BitBuf bits, const size_t num_bits)
{
    self->bitbuf |= bits << self->bitcount;
    self->bitcount += num_bits;
}

/**
  @brief Flush bits that fit into bytes
*/
static inline void io_bw_flush_bits(IoBitWriter *self)
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
static inline void io_bw_flush_bits_remaining(IoBitWriter *self)
{
    io_bw_flush_bits(self);
    io_write(self->writer, (uint8_t *)&self->bitbuf, 1);
    self->bitcount = 0;
    self->bitbuf = 0;
}

/**
  @brief Align the bitstream on a byte boundary.
*/
static inline void
io_bw_align_bitstream(IoBitWriter *self)
{
    self->bitcount += -self->bitcount & 7;
    io_bw_flush_bits(self);
}
