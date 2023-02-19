#include <brutal-debug>

#include "constants.h"
#include "errors.h"
#include "huff.h"
#include "inflate.h"

static const uint16_t LENS_BASE[30] = {/* Length codes 257..285 base */
                                       3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
                                       35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0};
static const uint16_t LENS_BITS[30] = {/* Length codes 257..285 extra */
                                       0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                                       1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
                                       4, 4, 4, 4, 5, 5, 5, 5, 0, 127};
static const uint16_t DIST_BASE[30] = {/* Distance codes 0..29 base */
                                       1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                                       257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
                                       8193, 12289, 16385, 24577};
static const uint16_t DIST_BITS[32] = {/* Distance codes 0..29 extra */
                                       0, 0, 0, 0, 1, 1, 2, 2, 3, 3,
                                       4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
                                       9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

typedef struct
{
    HuffTree length_tree;
    HuffTree dist_tree;
    IoWindow window;
    IoRBits bit_reader;
    IoWriter writer;
} DeflateDecompressor;

IoResult deflate_decompress_data(uint8_t const *in, size_t in_len, uint8_t *out, size_t out_len)
{
    // Input
    IoMem in_view;
    io_mem_init(&in_view, in_len, in);
    IoReader reader = io_mem_reader(&in_view);

    // Output
    IoMem out_view;
    io_mem_init(&out_view, out_len, out);
    IoWriter writer = io_mem_writer(&out_view);

    return deflate_decompress_stream(writer, reader);
}

MaybeError build_dynamic_huff_trees(DeflateDecompressor *ctx)
{
    // 288 symbols for lengths, 32 symbols for distance
    uint8_t lengths[288 + 32];

    // Special ordering of code length codes
    static const uint16_t order[DEFLATE_NUM_PRECODE_SYMS] =
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    IoRBits *bit_reader = &ctx->bit_reader;
    io_rbits_ensure_bits(bit_reader, 14);
    // Get 5 bits HLIT (257-286)
    uint32_t hlit = io_rbits_pop_bits(bit_reader, 5) + 257;
    // Get 5 bits HDIST (1-32)
    uint32_t hdist = io_rbits_pop_bits(bit_reader, 5) + 1;
    // Get 4 bits HCLEN (4-19)
    uint32_t hclen = io_rbits_pop_bits(bit_reader, 4) + 4;

    //  The RFC 1951 limits the range of HLIT to 286, but lists HDIST as range
    //  1-32, even though distance codes 30 and 31 have no meaning. While
    //  we could allow the full range of HLIT and HDIST to make it possible
    //  to decode the fixed trees with this function, we consider it an
    //  error here.
    //  See also: https://github.com/madler/zlib/issues/82
    if (hlit > DEFLATE_MAX_NUM_LITLEN_SYMS || hdist > DEFLATE_MAX_NUM_OFFSET_SYMS)
    {
        return ERR(MaybeError, ERR_TOO_MANY_SYMBOLS);
    }

    // Initialize lengths with zeros
    for (size_t i = 0; i < DEFLATE_NUM_PRECODE_SYMS; i++)
    {
        lengths[i] = 0;
    }

    // 3 bits per code
    for (size_t i = 0; i < hclen; i++)
    {
        io_rbits_ensure_bits(bit_reader, 3);
        lengths[order[i]] = io_rbits_pop_bits(bit_reader, 3);
    }

    TRY(MaybeError, build_huff_tree(&ctx->length_tree, lengths, DEFLATE_NUM_PRECODE_SYMS));

    HuffDecoder dec;
    huff_decoder_init(&dec, bit_reader, &ctx->length_tree);
    for (size_t num = 0; num < hdist + hlit;)
    {
        uint16_t symbol = huff_decoder_next(&dec);
        uint32_t repeat_count = 0;

        switch (symbol)
        {
        // 3-6
        case 16:
            io_rbits_ensure_bits(bit_reader, 2);
            repeat_count = io_rbits_pop_bits(bit_reader, 2) + 3;
            // Last element will get repeated
            symbol = lengths[num - 1];
            break;
        // 3-10
        case 17:
            io_rbits_ensure_bits(bit_reader, 3);
            symbol = 0;
            repeat_count = io_rbits_pop_bits(bit_reader, 3) + 3;
            break;
        // 11 - 138
        case 18:
            io_rbits_ensure_bits(bit_reader, 7);
            symbol = 0;
            repeat_count = io_rbits_pop_bits(bit_reader, 7) + 11;
            break;
        // 0-15 are literals
        default:
            repeat_count = 1;
            break;
        }

        if (repeat_count > hlit + hdist - num)
        {
            return ERROR(ERR_BAD_DATA);
        }

        for (uint32_t i = 0; i != repeat_count; i++)
        {
            lengths[num++] = symbol;
        }
    }

    if (lengths[256] == 0)
    {
        return ERROR(ERR_INVALID_CODE);
    }

    TRY(MaybeError, build_huff_tree(&ctx->length_tree, lengths, hlit));
    TRY(MaybeError, build_huff_tree(&ctx->dist_tree, lengths + hlit, hdist));

    return SUCCESS;
}

void build_static_huff_trees(DeflateDecompressor *ctx)
{
    HuffTree *lt = &ctx->length_tree;
    HuffTree *dt = &ctx->dist_tree;

    /* Build fixed literal/length tree */
    for (size_t i = 0; i < 16; ++i)
    {
        lt->counts[i] = 0;
    }

    lt->counts[7] = 24;
    lt->counts[8] = 152;
    lt->counts[9] = 112;

    for (size_t i = 0; i < 24; ++i)
    {
        lt->symbols[i] = 256 + i;
    }
    for (size_t i = 0; i < 144; ++i)
    {
        lt->symbols[24 + i] = i;
    }
    for (size_t i = 0; i < 8; ++i)
    {
        lt->symbols[24 + 144 + i] = 280 + i;
    }
    for (size_t i = 0; i < 112; ++i)
    {
        lt->symbols[24 + 144 + 8 + i] = 144 + i;
    }

    lt->max_sym = 285;

    // Build fixed distance tree
    for (size_t i = 0; i < 16; ++i)
    {
        dt->counts[i] = 0;
    }

    dt->counts[5] = 32;

    for (size_t i = 0; i < 32; ++i)
    {
        dt->symbols[i] = i;
    }

    dt->max_sym = 29;
}

IoResult deflate_decompress_block(DeflateDecompressor *ctx, bool *final_block)
{
    size_t decompressed_size = 0;
    io_rbits_ensure_bits(&ctx->bit_reader, 1 + 2);

    *final_block = io_rbits_pop_bits(&ctx->bit_reader, 1);
    uint32_t block_type = io_rbits_pop_bits(&ctx->bit_reader, 2);

    uint16_t len, nlen;

    if (block_type == DEFLATE_BLOCKTYPE_UNCOMPRESSED)
    {
        io_rbits_align(&ctx->bit_reader);

        TRY(IoResult, io_read(ctx->bit_reader.reader, (uint8_t *)&len, sizeof(uint16_t)));
        TRY(IoResult, io_read(ctx->bit_reader.reader, (uint8_t *)&nlen, sizeof(uint16_t)));

        assert_equal(len, (uint16_t)~nlen);

        decompressed_size = TRY(IoResult, io_copy_n(ctx->bit_reader.reader, ctx->writer, len));
    }
    else if (block_type == DEFLATE_BLOCKTYPE_STATIC_HUFFMAN || block_type == DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN)
    {
        if (block_type == DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN)
            TRY(IoResult, build_dynamic_huff_trees(ctx));
        else
            build_static_huff_trees(ctx);

        HuffDecoder len_dec, dist_dec;
        huff_decoder_init(&len_dec, &ctx->bit_reader, &ctx->length_tree);
        huff_decoder_init(&dist_dec, &ctx->bit_reader, &ctx->dist_tree);

        while (true)
        {
            uint16_t symbol = huff_decoder_next(&len_dec);

            // Literal
            if (symbol < 256)
            {
                uint8_t val = symbol;
                TRY(IoResult, io_write(ctx->writer, &val, 1));
                decompressed_size++;
            }
            // End of block
            else if (symbol == 256)
            {
                break;
            }
            else
            {
                // Check sym is within range and distance tree is not empty
                if (symbol > ctx->length_tree.max_sym || symbol - 257 > 28 || ctx->dist_tree.max_sym == -1)
                {
                    return ERR(IoResult, ERR_BAD_DATA);
                }

                symbol -= 257;
                io_rbits_ensure_bits(&ctx->bit_reader, LENS_BITS[symbol]);
                uint32_t length = LENS_BASE[symbol] + io_rbits_pop_bits(&ctx->bit_reader, LENS_BITS[symbol]);

                uint16_t dist = huff_decoder_next(&dist_dec);
                io_rbits_ensure_bits(&ctx->bit_reader, DIST_BITS[dist]);
                uint32_t offset = DIST_BASE[dist] + io_rbits_pop_bits(&ctx->bit_reader, DIST_BITS[dist]);
                // Copy match
                decompressed_size += length;
                for (size_t i = 0; i < length; ++i)
                {
                    uint8_t val = io_window_peek_from_back(&ctx->window, offset);
                    TRY(IoResult, io_write(ctx->writer, &val, 1));
                }
            }
        }
    }
    else
    {
        return ERR(IoResult, ERR_UNKNOWN_BLOCK);
    }

    return OK(IoResult, decompressed_size);
}

IoResult deflate_decompress_stream(IoWriter writer, IoReader reader)
{
    size_t total = 0;
    static_assert(1 + 2 + 5 + 5 + 4 < BITBUF_NBITS, "BitReader can't hold enough bits");

    bool final_block;

    DeflateDecompressor ctx;
    io_rbits_init(&ctx.bit_reader, reader);
    // TODO: windowsize can be smaller (see RFC 1950)
    io_window_init(&ctx.window, writer, 0x8000, alloc_global());
    ctx.writer = io_window_writer(&ctx.window);

    do
    {
        total += TRY(IoResult, deflate_decompress_block(&ctx, &final_block));
    } while (!final_block);

    io_window_flush_all(&ctx.window);
    io_window_deinit(&ctx.window);

    return OK(IoResult, total);
}
