#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/base/std.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

typedef uint32_t BitBuf;
#define BITBUF_NBITS (8 * sizeof(bitbuf_t))

typedef struct
{
    /* The output stream where we flush our bits into */
    IoWriter *writer;
    /* Bits that haven't yet been written to the output buffer */
    BitBuf bitbuf;
    /* Number of bits currently held in @bitbuf */
    unsigned bitcount;
} BitWriter;

static inline void io_bw_init(BitWriter *self, IoWriter *writer)
{
    self->writer = writer;
    self->bitbuf = 0;
    self->bitcount = 0;
}

/*
 * Add some bits to the bitbuffer variable of the output bitstream.  The caller
 * must make sure there is enough room.
 */
static inline void
io_bw_add_bits(BitWriter *self,
               const BitBuf bits, const unsigned num_bits)
{
    self->bitbuf |= bits << self->bitcount;
    self->bitcount += num_bits;
}

static inline void io_bw_flush_bits(BitWriter *self)
{
    /* Flush a byte at a time. */
    while (self->bitcount >= 8)
    {
        io_write(self->writer, (uint8_t *)&self->bitbuf, 1);
        self->bitcount -= 8;
        self->bitbuf >>= 8;
    }
}

static inline void io_bw_flush_bits_remaining(BitWriter *self)
{
    /* Flush a byte at a time. */
    while (self->bitcount >= 0)
    {
        io_write(self->writer, (uint8_t *)&self->bitbuf, 1);
        self->bitcount -= 8;
        self->bitbuf >>= 8;
    }
}

/* Align the bitstream on a byte boundary. */
static inline void
io_bw_align_bitstream(BitWriter *self)
{
    self->bitcount += -self->bitcount & 7;
    io_bw_flush_bits(self);
}
