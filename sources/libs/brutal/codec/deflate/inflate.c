#include <brutal/codec/deflate/constants.h>
#include <brutal/codec/deflate/huff.h>
#include <brutal/codec/deflate/inflate.h>
#include <brutal/debug/assert.h>
#include <brutal/io/bit_read.h>
#include <brutal/io/mem_view.h>

#define ERR_UNKNOWN_BLOCK make_error(ERR_KIND_INVALID_DATA, "unknown-blocktype")

IoResult deflate_decompress_data(const uint8_t *in, size_t in_len, const uint8_t *out, size_t out_len)
{
    // Input
    MemView in_view;
    mem_view_init(&in_view, in_len, in);
    IoReader reader = mem_view_reader(&in_view);

    // Output
    MemView out_view;
    mem_view_init(&out_view, out_len, out);
    IoWriter writer = mem_view_writer(&out_view);

    return deflate_decompress_stream(writer, reader);
}

IoResult deflate_decompress_block(IoWriter writer, BitReader *bit_reader, bool *final_block)
{
    size_t decompressed_size = 0;
    io_br_ensure_bits(bit_reader, 1 + 2);

    *final_block = io_br_pop_bits(bit_reader, 1);
    unsigned block_type = io_br_pop_bits(bit_reader, 2);

    uint16_t len, nlen;

    /* The order in which precode lengths are stored.  */
    static const uint8_t deflate_precode_lens_permutation[DEFLATE_NUM_PRECODE_SYMS] = {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    switch (block_type)
    {
    case DEFLATE_BLOCKTYPE_UNCOMPRESSED:
        io_br_align(bit_reader);

        TRY(IoResult, io_read(bit_reader->reader, (uint8_t *)&len, sizeof(uint16_t)));
        TRY(IoResult, io_read(bit_reader->reader, (uint8_t *)&nlen, sizeof(uint16_t)));

        assert_equal(len, (uint16_t)~nlen);

        decompressed_size = TRY(IoResult, io_copy_range(bit_reader->reader, writer, len));
        break;
    case DEFLATE_BLOCKTYPE_STATIC_HUFFMAN:
        break;
    case DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN:

        break;
    default:
        return ERR(IoResult, ERR_UNKNOWN_BLOCK);
    }

    return OK(IoResult, decompressed_size);
}

IoResult deflate_decompress_stream(IoWriter writer, IoReader reader)
{
    size_t total = 0;
    static_assert(1 + 2 + 5 + 5 + 4 < BITBUF_NBITS, "BitReader can't hold enough bits");

    BitReader bit_reader;
    io_br_init(&bit_reader, reader);
    bool final_block;

    do
    {
        total += TRY(IoResult, deflate_decompress_block(writer, &bit_reader, &final_block));
    } while (!final_block);

    return OK(IoResult, total);
}
