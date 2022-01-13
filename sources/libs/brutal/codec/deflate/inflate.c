#include <brutal/codec/deflate/constants.h>
#include <brutal/codec/deflate/errors.h>
#include <brutal/codec/deflate/huff.h>
#include <brutal/codec/deflate/inflate.h>
#include <brutal/debug/assert.h>
#include <brutal/io/bit_read.h>
#include <brutal/io/mem_view.h>

#define MAXBITS 15

static const uint16_t LENS_BASE[31] = {/* Length codes 257..285 base */
                                       3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
                                       35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
static const uint16_t LENS_EXTRA[31] = {/* Length codes 257..285 extra */
                                        16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18,
                                        19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 77, 202};
static const uint16_t DIST_BASE[32] = {/* Distance codes 0..29 base */
                                       1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                                       257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
                                       8193, 12289, 16385, 24577, 0, 0};
static const uint16_t DIST_EXTRA[32] = {/* Distance codes 0..29 extra */
                                        16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
                                        23, 23, 24, 24, 25, 25, 26, 26, 27, 27,
                                        28, 28, 29, 29, 64, 64};

typedef struct
{
    uint32_t have;
    int32_t back;
    uint32_t length;
    // Static & Dynamic Huffman
    Code const *len_code;  /* starting table for length/literal codes */
    Code const *dist_code; /* starting table for distance codes */
    uint32_t len_bits;     /* index bits for lencode */
    uint32_t dist_bits;    /* index bits for distcode */
    // Dynamic Huffman
    uint32_t hlit;                       /* number of code length code lengths */
    uint32_t hdist;                      /* number of length code lengths */
    uint32_t hclen;                      /* number of distance code lengths */
    uint16_t lens[320];                  /* temporary storage for code lengths */
    uint16_t work[DEFLATE_MAX_NUM_SYMS]; /* work area for code table building */
    Code codes[ENOUGH];                  /* space for code tables */
    Code *next;                          /* next available space in codes[] */
} DeflateDecompressionContext;

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

// Should we maybe inline this?
MaybeError inflate_table(uint16_t *lens, uint32_t codes, Code **table, uint32_t *bits, uint16_t *work,
                         const uint16_t *base, const uint16_t *extra, const uint32_t match, const uint32_t enough)
{
    uint16_t count[MAXBITS + 1]; /* number of codes of each length */
    uint16_t offs[MAXBITS + 1];  /* offsets in table for each length */
    Code here;

    // Create a canonical huffman code: https://en.wikipedia.org/wiki/Canonical_Huffman_code
    /* accumulate lengths for codes (assumes lens[] all in 0..MAXBITS) */
    for (uint8_t len = 0; len <= MAXBITS; len++)
        count[len] = 0;
    for (uint32_t sym = 0; sym < codes; sym++)
        count[lens[sym]]++;

    /* bound code lengths, force root to be within code lengths */
    uint32_t root = *bits;
    uint32_t min, max;
    for (max = MAXBITS; max >= 1; max--)
        if (count[max] != 0)
            break;
    if (root > max)
        root = max;
    if (max == 0)
    {                                         /* no symbols to code at all */
        here.op = (uint8_t)OP_INVALID_MARKER; /* invalid code marker */
        here.bits = (uint8_t)1;
        here.val = (uint16_t)0;
        *(*table)++ = here; /* make a table to force an error */
        *(*table)++ = here;
        *bits = 1;
        return SUCCESS; /* no symbols, but wait for decoding to report error */
    }
    for (min = 1; min < max; min++)
        if (count[min] != 0)
            break;
    if (root < min)
        root = min;

    /* check for an over-subscribed or incomplete set of lengths */
    int32_t left = 1;
    for (uint32_t len = 1; len <= MAXBITS; len++)
    {
        left <<= 1;
        left -= count[len];
        if (left < 0)
            return ERROR(ERR_OVER_SUBSCRIBED); /* over-subscribed */
    }
    if (left > 0)
        return ERROR(ERR_INCOMPLETE_SET); /* incomplete set */

    /* generate offsets into symbol table for each length for sorting */
    offs[1] = 0;
    for (uint32_t len = 1; len < MAXBITS; len++)
        offs[len + 1] = offs[len] + count[len];

    /* sort symbols by length, by symbol order within each length */
    for (uint32_t sym = 0; sym < codes; sym++)
        if (lens[sym] != 0)
            work[offs[lens[sym]]++] = (uint16_t)sym;

    /* initialize state for loop */
    uint32_t huff = 0;          /* starting code */
    uint32_t sym = 0;           /* starting code symbol */
    uint32_t len = min;         /* starting code length */
    Code *next = *table;        /* current table to fill in */
    uint32_t curr = root;       /* current table index bits */
    uint32_t drop = 0;          /* current bits to drop from code for index */
    uint32_t low = UINT32_MAX;  /* trigger new sub-table when len > root */
    uint32_t used = 1U << root; /* use root table entries */
    uint32_t mask = used - 1;   /* mask for comparing low */

    /* check available table space */
    if (used > enough)
        return ERROR(ERR_NOT_ENOUGH_TABLE_SPACE);

    /* process all codes and make table entries */
    for (;;)
    {
        here.bits = (uint8_t)(len - drop);
        if (work[sym] + 1U < match) // Literal
        {
            here.op = (uint8_t)OP_LITERAL_MARKER;
            here.val = work[sym];
        }
        else if (work[sym] >= match) // Distance code
        {
            here.op = (uint8_t)(extra[work[sym] - match]);
            here.val = base[work[sym] - match];
        }
        else // End of block
        {
            here.op = (uint8_t)(OP_INVALID_MARKER + OP_END_MARKER);
            here.val = 0;
        }

        /* replicate for those indices with low len bits equal to huff */
        uint32_t incr = 1U << (len - drop);
        uint32_t fill = 1U << curr;
        min = fill; /* save offset to next table */
        do
        {
            fill -= incr;
            next[(huff >> drop) + fill] = here;
        } while (fill != 0);

        /* backwards increment the len-bit code huff */
        incr = 1U << (len - 1);
        while (huff & incr)
            incr >>= 1;
        if (incr != 0)
        {
            huff &= incr - 1;
            huff += incr;
        }
        else
            huff = 0;

        /* go to next symbol, update count, len */
        sym++;
        if (--(count[len]) == 0)
        {
            if (len == max)
                break;
            len = lens[work[sym]];
        }

        /* create new sub-table if needed */
        if (len > root && (huff & mask) != low)
        {
            /* if first time, transition to sub-tables */
            if (drop == 0)
                drop = root;

            /* increment past last table */
            next += min; /* here min is 1 << curr */

            /* determine length of next table */
            curr = len - drop;
            left = (int)(1 << curr);
            while (curr + drop < max)
            {
                left -= count[curr + drop];
                if (left <= 0)
                    break;
                curr++;
                left <<= 1;
            }

            /* check for enough space */
            used += 1U << curr;
            /* check available table space */
            if (used > enough)
                return ERROR(ERR_NOT_ENOUGH_TABLE_SPACE);

            /* point entry in root table to sub-table */
            low = huff & mask;
            (*table)[low].op = (uint8_t)curr;
            (*table)[low].bits = (uint8_t)root;
            (*table)[low].val = (uint16_t)(next - *table);
        }
    }

    /* fill in remaining table entry if code is incomplete (guaranteed to have
       at most one remaining entry, since if the code is incomplete, the
       maximum code length that was allowed to get this far is one bit) */
    if (huff != 0)
    {
        here.op = (uint8_t)OP_INVALID_MARKER; /* invalid code marker */
        here.bits = (uint8_t)(len - drop);
        here.val = (uint16_t)0;
        next[huff] = here;
    }

    /* set return parameters */
    *table += used;
    *bits = root;

    return SUCCESS;
}

MaybeError inflate_table_precode(uint16_t *lens, uint32_t codes, Code **table, uint32_t *bits, uint16_t *work)
{
    return inflate_table(lens, codes, table, bits, work, work, work, 20, UINT32_MAX);
}

MaybeError inflate_table_length(uint16_t *lens, uint32_t codes, Code **table, uint32_t *bits, uint16_t *work)
{
    return inflate_table(lens, codes, table, bits, work, LENS_BASE, LENS_EXTRA, 257, ENOUGH_LENS);
}

MaybeError inflate_table_distance(uint16_t *lens, uint32_t codes, Code **table, uint32_t *bits, uint16_t *work)
{
    return inflate_table(lens, codes, table, bits, work, DIST_BASE, DIST_EXTRA, 0, ENOUGH_DISTS);
}

MaybeError build_dynamic_huffman_alphabet(DeflateDecompressionContext *ctx, BitReader *bit_reader)
{
    static const uint16_t order[DEFLATE_NUM_PRECODE_SYMS] = /* permutation of code lengths */
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    // Read the lens
    io_br_ensure_bits(bit_reader, 14);
    ctx->hlit = io_br_pop_bits(bit_reader, 5) + 257;
    ctx->hdist = io_br_pop_bits(bit_reader, 5) + 1;
    ctx->hclen = io_br_pop_bits(bit_reader, 4) + 4;

    if (ctx->hlit > DEFLATE_MAX_NUM_LITLEN_SYMS || ctx->hdist > DEFLATE_MAX_NUM_OFFSET_SYMS)
    {
        return ERR(MaybeError, ERR_TOO_MANY_SYMBOLS);
    }

    // 3 bits per code
    uint32_t i;
    for (i = 0; i < ctx->hclen; i++)
    {
        io_br_ensure_bits(bit_reader, 3);
        ctx->lens[order[i]] = io_br_pop_bits(bit_reader, 3);
    }

    for (; i < DEFLATE_NUM_PRECODE_SYMS; i++)
    {
        ctx->lens[order[i]] = 0;
    }

    ctx->next = ctx->codes;
    ctx->len_code = ctx->next;
    ctx->len_bits = 7;
    TRY(MaybeError, inflate_table_precode(ctx->lens, DEFLATE_NUM_PRECODE_SYMS, &ctx->next, &ctx->len_bits, ctx->work));

    ctx->have = 0;
    HuffDecoder dec;
    huff_dec_init(&dec, bit_reader, ctx->len_code, ctx->len_bits);
    while (ctx->have < ctx->hdist + ctx->hlit)
    {
        Code here = TRY(MaybeError, huff_dec_get_code(&dec));

        // Everything below 16 corresponds directly to a codelength. See https://tools.ietf.org/html/rfc1951#section-3.2.7
        if (here.val < 16)
        {
            ctx->lens[ctx->have++] = here.val;
        }
        else
        {
            uint32_t repeat_count = 0;
            uint32_t len_to_repeat = 0;

            switch (here.val)
            {
            // 3-6
            case 16:
                io_br_ensure_bits(bit_reader, 2);
                repeat_count = io_br_pop_bits(bit_reader, 2) + 3;
                // Last element will get repeated
                len_to_repeat = ctx->lens[ctx->have - 1];
                break;
            // 3-10
            case 17:
                io_br_ensure_bits(bit_reader, 3);
                repeat_count = io_br_pop_bits(bit_reader, 3) + 3;
                break;
            // 11 - 138
            case 18:
                io_br_ensure_bits(bit_reader, 7);
                repeat_count = io_br_pop_bits(bit_reader, 7) + 11;
                break;
            }

            assert_lower_equal(ctx->have, ctx->hdist + ctx->hlit);

            for (i = 0; i != repeat_count; i++)
            {
                ctx->lens[ctx->have++] = (uint16_t)len_to_repeat;
            }
        }
    }

    if (ctx->lens[256] == 0)
    {
        return ERR(MaybeError, ERR_INVALID_CODE);
    }

    ctx->next = ctx->codes;
    ctx->len_code = ctx->next;
    ctx->len_bits = 9;
    TRY(MaybeError, inflate_table_length(ctx->lens, ctx->hlit, &ctx->next, &ctx->len_bits, ctx->work));
    ctx->dist_code = ctx->next;
    ctx->dist_bits = 6;
    TRY(MaybeError, inflate_table_distance(ctx->lens + ctx->hlit, ctx->hdist, &ctx->next, &ctx->dist_bits, ctx->work));
    return SUCCESS;
}

void build_static_huffman_alphabet(DeflateDecompressionContext *ctx)
{
    static bool initialized = false;
    static Code *len_fix, *dist_fix;
    static Code fixed[544];

    if (!initialized)
    {
        uint32_t sym, bits;
        static Code *next;

        /* literal/length table */
        sym = 0;
        while (sym < 144)
            ctx->lens[sym++] = 8;
        while (sym < 256)
            ctx->lens[sym++] = 9;
        while (sym < 280)
            ctx->lens[sym++] = 7;
        while (sym < 288)
            ctx->lens[sym++] = 8;
        next = fixed;
        len_fix = next;
        bits = 9;
        inflate_table_length(ctx->lens, 288, &(next), &(bits), ctx->work);

        /* distance table */
        sym = 0;
        while (sym < 32)
            ctx->lens[sym++] = 5;
        dist_fix = next;
        bits = 5;
        inflate_table_distance(ctx->lens, 32, &(next), &(bits), ctx->work);
        initialized = true;
    }

    ctx->len_code = len_fix;
    ctx->len_bits = 9;
    ctx->dist_code = dist_fix;
    ctx->dist_bits = 5;
}

IoResult deflate_decompress_block(DeflateDecompressionContext *ctx, IoWriter writer, BitReader *bit_reader, bool *final_block)
{
    size_t decompressed_size = 0;
    io_br_ensure_bits(bit_reader, 1 + 2);

    *final_block = io_br_pop_bits(bit_reader, 1);
    uint32_t block_type = io_br_pop_bits(bit_reader, 2);

    uint16_t len, nlen;

    if (block_type == DEFLATE_BLOCKTYPE_UNCOMPRESSED)
    {
        io_br_align(bit_reader);

        TRY(IoResult, io_read(bit_reader->reader, (uint8_t *)&len, sizeof(uint16_t)));
        TRY(IoResult, io_read(bit_reader->reader, (uint8_t *)&nlen, sizeof(uint16_t)));

        assert_equal(len, (uint16_t)~nlen);

        decompressed_size = TRY(IoResult, io_copy_range(bit_reader->reader, writer, len));
    }
    else if (block_type == DEFLATE_BLOCKTYPE_STATIC_HUFFMAN || block_type == DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN)
    {
        if (block_type == DEFLATE_BLOCKTYPE_DYNAMIC_HUFFMAN)
            TRY(IoResult, build_dynamic_huffman_alphabet(ctx, bit_reader));
        else
            build_static_huffman_alphabet(ctx);

        HuffDecoder len_decoder, dist_decoder;
        huff_dec_init(&len_decoder, bit_reader, ctx->len_code, ctx->len_bits);
        huff_dec_init(&dist_decoder, bit_reader, ctx->dist_code, ctx->dist_bits);

        while (true)
        {
            ctx->back = 0;
            Code symbol = TRY(IoResult, huff_dec_get_code(&len_decoder));

            if (symbol.op && (symbol.op & 0xf0) == 0)
            {
                Code last = symbol;
                symbol = TRY(IoResult, huff_dec_get_code_offset(&len_decoder, last));
                ctx->back += last.bits;
            }
            ctx->back += symbol.bits;
            ctx->length = symbol.val;

            // Literals
            if (symbol.op == OP_LITERAL_MARKER)
            {
                uint8_t val = ctx->length;
                TRY(IoResult, io_write(writer, &val, 1));
                decompressed_size++;
                continue;
            }
            // First check for end marker
            if (symbol.op & OP_END_MARKER)
            {
                ctx->back = -1;
                break;
            }
            if (symbol.op & OP_INVALID_MARKER)
            {
                return ERR(IoResult, ERR_INVALID_CODE);
            }
            // Distance codes
            Code dist = TRY(IoResult, huff_dec_get_code(&dist_decoder));
            if (dist.op && (dist.op & 0xf0) == 0)
            {
                Code last = dist;
                dist = TRY(IoResult, huff_dec_get_code_offset(&dist_decoder, last));
                ctx->back += last.bits;
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

    BitReader bit_reader;
    io_br_init(&bit_reader, reader);
    bool final_block;

    DeflateDecompressionContext ctx;

    do
    {
        total += TRY(IoResult, deflate_decompress_block(&ctx, writer, &bit_reader, &final_block));
    } while (!final_block);

    return OK(IoResult, total);
}
