#pragma once

#include <brutal/io.h>
#include <brutal/io/bit_write.h>

typedef struct _DeflateCompressionContext
{
    int compression_level;
    int min_size_to_compress;
    IoResult (*compress_block_impl)(struct _DeflateCompressionContext *c, BitWriter *bit_writer, const uint8_t *in,
                                    size_t in_nbytes, bool last);
} DeflateCompressionContext;

/** 
 * Allocate our compression context
 * @param compression_level The rate at which we want to compress (0 none, 9 best compression)
 */
DeflateCompressionContext *deflate_alloc(int compression_level);
/**
 * Free the compression context
 */
void deflate_free(DeflateCompressionContext *ctx);
/**
 * Compress an entire stream
 * @param ctx The compression context used for this operation
 * @param writer The destination where we write compressed data
 * @param reader The source where we read the uncompressed data from
 */
IoResult deflate_compress_stream(DeflateCompressionContext *ctx, IoWriter *writer, IoReader *reader);
/**
 * Decompress an entire stream
 * @param writer The destination where we write the uncompressed data
 * @param reader The source where we read the compressed data from
 */
IoResult deflate_decompress_stream(IoWriter *writer, IoReader *reader);
