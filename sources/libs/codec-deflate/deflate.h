#pragma once

#include <brutal-io/wbits.h>

typedef struct _DeflateCompressor
{
    int compression_level;
    int min_size_to_compress;
    Alloc *alloc;
    IoResult (*compress_block_impl)(struct _DeflateCompressor *c, uint8_t const *in,
                                    size_t in_nbytes, bool last);
    IoWBits bit_writer;
} DeflateCompressor;

/**
  @brief Allocate our compression context
  @param ctx The context which we want to initialize
  @param compression_level The rate at which we want to compress (0 none, 9 best compression)
  @param alloc The heap object which is used for internal allocations
*/
void deflate_init(DeflateCompressor *ctx, int compression_level, Alloc *alloc);

/**
  @brief Free the compression context
*/
void deflate_deinit(DeflateCompressor *ctx);

/**
  @brief Compress an entire stream
  @param ctx The compression context used for this operation
  @param in The input data to compress
  @param in_len The size of the input data
  @param out The output buffer where we can store our data
  @param out_len The size of the output buffer
  @return The number of bytes written to the \p out buffer (compressed size)
*/
IoResult deflate_compress_data(DeflateCompressor *ctx, uint8_t const *in, size_t in_len, uint8_t *out, size_t out_len);

/**
  @brief Compress an entire stream
  @param ctx The compression context used for this operation
  @param writer The destination where we write compressed data
  @param reader The source where we read the uncompressed data from
  @return The number of bytes written to the \p writer stream (compressed size)
*/
IoResult deflate_compress_stream(DeflateCompressor *ctx, IoWriter writer, IoReader reader);
