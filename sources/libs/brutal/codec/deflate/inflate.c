#include <brutal/codec/deflate/constants.h>
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

    return deflate_decompress_stream(&writer, &reader);
}

IoResult deflate_decompress_stream(IoWriter *writer, IoReader *reader)
{
    UNUSED(writer);
    static_assert(1 + 2 + 5 + 5 + 4 < BITBUF_NBITS, "BitReader can't hold enough bits");

    BitReader bit_reader;
    io_br_init(&bit_reader, reader);
    io_br_ensure_bits(&bit_reader, 1 + 2 + 5 + 5 + 4);

    bool is_final = io_br_pop_bits(&bit_reader, 1);
    UNUSED(is_final);
    unsigned block_type = io_br_pop_bits(&bit_reader, 2);

    uint16_t len, nlen;

    switch (block_type)
    {
    case DEFLATE_BLOCKTYPE_UNCOMPRESSED:
        io_br_align(&bit_reader);

        io_read(bit_reader.reader, (uint8_t *)&len, sizeof(uint16_t));
        io_read(bit_reader.reader, (uint8_t *)&nlen, sizeof(uint16_t));

        assert_equal(len, ~nlen);
        break;

    default:
        return ERR(IoResult, ERR_UNKNOWN_BLOCK);
    }

    return OK(IoResult, 0);
}
