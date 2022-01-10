#include <brutal/alloc.h>
#include <brutal/codec/deflate/constants.h>
#include <brutal/codec/deflate/deflate.h>
#include <brutal/io/mem_view.h>

// Block compressions
IoResult deflate_compress_block_uncompressed(DeflateCompressionContext *ctx, BitWriter *bit_writer, const uint8_t *in, size_t in_nbytes, bool last);

void deflate_init(DeflateCompressionContext *ctx, int level, Alloc *alloc)
{
    ctx->alloc = alloc;
    ctx->compression_level = level;
    ctx->min_size_to_compress = 56 - (ctx->compression_level * 4);

    if (level == 0)
    {
        ctx->compress_block_impl = deflate_compress_block_uncompressed;
    }
    else
    {
        //TODO: IMPLEMENT OTHER MODES
    }
}

void deflate_deinit(DeflateCompressionContext *ctx)
{
    // TODO: free arrays later
    UNUSED(ctx);
}

/* Write the header fields common to all DEFLATE block types. */
void deflate_write_block_header(BitWriter *bit_writer,
                                bool is_final_block, unsigned block_type)
{
    io_bw_add_bits(bit_writer, is_final_block, 1);
    io_bw_add_bits(bit_writer, block_type, 2);
    io_bw_flush_bits(bit_writer);
}

IoResult deflate_compress_block_uncompressed(DeflateCompressionContext *ctx, BitWriter *bit_writer, const uint8_t *in, size_t in_nbytes, bool is_last)
{
    UNUSED(ctx);
    deflate_write_block_header(bit_writer, is_last,
                               DEFLATE_BLOCKTYPE_UNCOMPRESSED);
    io_bw_align_bitstream(bit_writer);

    uint16_t block_len = in_nbytes;
    io_write(bit_writer->writer, (uint8_t *)&block_len, sizeof(uint16_t));
    uint16_t block_len_inv = ~in_nbytes;
    io_write(bit_writer->writer, (uint8_t *)&block_len_inv, sizeof(uint16_t));
    io_write(bit_writer->writer, in, in_nbytes);

    // TODO: this calculation can be wrong
    return OK(IoResult, in_nbytes + 5);
}

IoResult deflate_compress_data(DeflateCompressionContext *ctx, const uint8_t *in, size_t in_len, const uint8_t *out, size_t out_len)
{
    // Input
    MemView in_view;
    mem_view_init(&in_view, in_len, in);
    IoReader reader = mem_view_reader(&in_view);

    // Output
    MemView out_view;
    mem_view_init(&out_view, out_len, out);
    IoWriter writer = mem_view_writer(&out_view);

    return deflate_compress_stream(ctx, &writer, &reader);
}

IoResult deflate_compress_stream(DeflateCompressionContext *ctx, IoWriter *writer, IoReader *reader)
{
    size_t total = 0;
    bool last = false;

    BitWriter bit_writer;
    io_bw_init(&bit_writer, writer);

    do
    {
        // Maximum blocksize is 65535
        uint8_t buf[UINT16_MAX];
        size_t read = TRY(IoResult, io_read(reader, buf, sizeof(buf)));
        last = read < sizeof(buf);
        total += TRY(IoResult, (*ctx->compress_block_impl)(ctx, &bit_writer, buf, read, last));
    } while (!last);

    return OK(IoResult, total);
}
