#include <brutal/codec/deflate/constants.h>
#include <brutal/codec/deflate/errors.h>
#include <brutal/codec/deflate/huff.h>
#include <brutal/codec/deflate/inflate.h>
#include <brutal/debug/assert.h>
#include <brutal/io/bit_read.h>
#include <brutal/io/mem_view.h>

#define MAXBITS 15

// static const uint16_t BASE_LENS[31] = {/* Length codes 257..285 base */
//                                        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
//                                        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
// static const uint16_t BASE_DISTANCE[32] = {/* Distance codes 0..29 base */
//                                            1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
//                                            257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
//                                            8193, 12289, 16385, 24577, 0, 0};

typedef struct
{
    unsigned char op;   /* operation, extra bits, table bits */
    unsigned char bits; /* bits in this part of the code */
    unsigned short val; /* offset in table or code value */
} Code;

typedef struct
{
    unsigned lenbits;
    // DYNAMIC Huffman
    unsigned hlit;                       /* number of code length code lengths */
    unsigned hdist;                      /* number of length code lengths */
    unsigned hclen;                      /* number of distance code lengths */
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

MaybeError inflate_table(uint16_t *lens, unsigned codes, Code **table, unsigned *bits, uint16_t *work,
                         const uint16_t *base, const uint16_t *extra, const unsigned match, const unsigned enough)
{
    unsigned short count[MAXBITS + 1]; /* number of codes of each length */
    unsigned short offs[MAXBITS + 1];  /* offsets in table for each length */
    Code here;

    // Create a canonical huffman code: https://en.wikipedia.org/wiki/Canonical_Huffman_code
    /* accumulate lengths for codes (assumes lens[] all in 0..MAXBITS) */
    for (unsigned len = 0; len <= MAXBITS; len++)
        count[len] = 0;
    for (unsigned sym = 0; sym < codes; sym++)
        count[lens[sym]]++;

    /* bound code lengths, force root to be within code lengths */
    unsigned root = *bits;
    unsigned min, max;
    for (max = MAXBITS; max >= 1; max--)
        if (count[max] != 0)
            break;
    if (root > max)
        root = max;
    if (max == 0)
    {                                /* no symbols to code at all */
        here.op = (unsigned char)64; /* invalid code marker */
        here.bits = (unsigned char)1;
        here.val = (unsigned short)0;
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
    unsigned left = 1;
    for (unsigned len = 1; len <= MAXBITS; len++)
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
    for (unsigned len = 1; len < MAXBITS; len++)
        offs[len + 1] = offs[len] + count[len];

    /* sort symbols by length, by symbol order within each length */
    for (unsigned sym = 0; sym < codes; sym++)
        if (lens[sym] != 0)
            work[offs[lens[sym]]++] = (unsigned short)sym;

    /* initialize state for loop */
    unsigned huff = 0;             /* starting code */
    unsigned sym = 0;              /* starting code symbol */
    unsigned len = min;            /* starting code length */
    Code *next = *table;           /* current table to fill in */
    unsigned curr = root;          /* current table index bits */
    unsigned drop = 0;             /* current bits to drop from code for index */
    unsigned low = (unsigned)(-1); /* trigger new sub-table when len > root */
    unsigned used = 1U << root;    /* use root table entries */
    unsigned mask = used - 1;      /* mask for comparing low */

    /* check available table space */
    if (used > enough)
        return ERROR(ERR_NOT_ENOUGH_TABLE_SPACE);

    /* process all codes and make table entries */
    for (;;)
    {
        /* create table entry */
        here.bits = (unsigned char)(len - drop);
        if (work[sym] + 1U < match)
        {
            here.op = (unsigned char)0;
            here.val = work[sym];
        }
        else if (work[sym] >= match)
        {
            here.op = (unsigned char)(extra[work[sym] - match]);
            here.val = base[work[sym] - match];
        }
        else
        {
            here.op = (unsigned char)(32 + 64); /* end of block */
            here.val = 0;
        }

        /* replicate for those indices with low len bits equal to huff */
        unsigned incr = 1U << (len - drop);
        unsigned fill = 1U << curr;
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
            (*table)[low].op = (unsigned char)curr;
            (*table)[low].bits = (unsigned char)root;
            (*table)[low].val = (unsigned short)(next - *table);
        }
    }

    /* fill in remaining table entry if code is incomplete (guaranteed to have
       at most one remaining entry, since if the code is incomplete, the
       maximum code length that was allowed to get this far is one bit) */
    if (huff != 0)
    {
        here.op = (unsigned char)64; /* invalid code marker */
        here.bits = (unsigned char)(len - drop);
        here.val = (unsigned short)0;
        next[huff] = here;
    }

    /* set return parameters */
    *table += used;
    *bits = root;

    return SUCCESS;
}

MaybeError inflate_table_precode(uint16_t *lens, unsigned codes, Code **table, unsigned *bits, uint16_t *work)
{
    return inflate_table(lens, codes, table, bits, work, work, work, 20, UINT32_MAX);
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
    io_br_ensure_bits(bit_reader, ctx->hclen * 3);
    unsigned i;
    for (i = 0; i < ctx->hclen; i++)
    {
        ctx->lens[order[i]] = io_br_pop_bits(bit_reader, 3);
    }

    for (; i < DEFLATE_NUM_PRECODE_SYMS; i++)
    {
        ctx->lens[order[i]] = 0;
    }

    ctx->next = ctx->codes;
    ctx->lenbits = 7;
    TRY(MaybeError, inflate_table_precode(ctx->lens, DEFLATE_NUM_PRECODE_SYMS, &ctx->next, &ctx->lenbits, ctx->work));
    return SUCCESS;
}

IoResult deflate_decompress_block(DeflateDecompressionContext *ctx, IoWriter writer, BitReader *bit_reader, bool *final_block)
{
    size_t decompressed_size = 0;
    io_br_ensure_bits(bit_reader, 1 + 2);

    *final_block = io_br_pop_bits(bit_reader, 1);
    unsigned block_type = io_br_pop_bits(bit_reader, 2);

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
            build_dynamic_huffman_alphabet(ctx, bit_reader);
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
