#include <brutal/base/attributes.h>
#include <brutal/codec/deflate/constants.h>
#include <brutal/codec/deflate/deflate.h>
#include <brutal/io/mem.h>

// Block compressions
IoResult deflate_compress_block_uncompressed(DeflateCompressor *ctx, uint8_t const *in, size_t in_nbytes, bool last);

void deflate_init(DeflateCompressor *ctx, int level, Alloc *alloc)
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
        // TODO: IMPLEMENT OTHER MODES
    }
}

void deflate_deinit(MAYBE_UNUSED DeflateCompressor *ctx)
{
    // TODO: free arrays later
}

/* Write the header fields common to all DEFLATE block types. */
void deflate_write_block_header(
    IoBitWriter *bit_writer,
    bool is_final_block,
    unsigned block_type)
{
    io_bw_add_bits(bit_writer, is_final_block, 1);
    io_bw_add_bits(bit_writer, block_type, 2);
    io_bw_flush_bits(bit_writer);
}

IoResult deflate_compress_block_uncompressed(DeflateCompressor *ctx, uint8_t const *in, size_t in_nbytes, bool is_last)
{
    deflate_write_block_header(&ctx->bit_writer, is_last,
                               DEFLATE_BLOCKTYPE_UNCOMPRESSED);
    io_bw_align_bitstream(&ctx->bit_writer);

    uint16_t block_len = in_nbytes;
    io_write(ctx->bit_writer.writer, (uint8_t *)&block_len, sizeof(uint16_t));
    uint16_t block_len_inv = ~in_nbytes;
    io_write(ctx->bit_writer.writer, (uint8_t *)&block_len_inv, sizeof(uint16_t));
    io_write(ctx->bit_writer.writer, in, in_nbytes);

    // TODO: this calculation can be wrong
    return OK(IoResult, in_nbytes + 5);
}

IoResult deflate_compress_data(DeflateCompressor *ctx, uint8_t const *in, size_t in_len, uint8_t const *out, size_t out_len)
{
    // Input
    IoMem in_view;
    io_mem_init(&in_view, in_len, in);
    IoReader reader = io_mem_reader(&in_view);

    // Output
    IoMem out_view;
    io_mem_init(&out_view, out_len, out);
    IoWriter writer = io_mem_writer(&out_view);

    return deflate_compress_stream(ctx, writer, reader);
}

IoResult deflate_compress_stream(DeflateCompressor *ctx, IoWriter writer, IoReader reader)
{
    size_t total = 0;
    bool last = false;

    io_bw_init(&ctx->bit_writer, writer);

    do
    {
        // Maximum blocksize is 65535
        uint8_t buf[UINT16_MAX];
        size_t read = TRY(IoResult, io_read(reader, buf, sizeof(buf)));
        last = read < sizeof(buf);
        total += TRY(IoResult, (*ctx->compress_block_impl)(ctx, buf, read, last));
    } while (!last);

    return OK(IoResult, total);
}
