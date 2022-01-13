#pragma once
#include <brutal/io/bit_read.h>

#define OP_LITERAL_MARKER 0
#define OP_END_MARKER 32
#define OP_INVALID_MARKER 64

typedef struct
{
    uint8_t op;   /* operation, extra bits, table bits */
    uint8_t bits; /* bits in this part of the code */
    uint16_t val; /* offset in table or code value */
} Code;

typedef Result(Error, Code) DecodeResult;

typedef struct
{
    BitReader *bit_reader;
    const Code *code;
    uint32_t bits;
} HuffDecoder;

void huff_dec_init(HuffDecoder *dec, BitReader *bit_reader, const Code *code, uint32_t bits)
{
    dec->bit_reader = bit_reader;
    dec->code = code;
    dec->bits = bits;
}

static inline DecodeResult huff_dec_get_code(HuffDecoder *dec)
{
    Code result;
    io_br_ensure_bits(dec->bit_reader, dec->bits);
    result = dec->code[io_br_pop_bits(dec->bit_reader, dec->bits)];
    io_br_ensure_bits(dec->bit_reader, result.bits);
    return OK(DecodeResult, result);
}

static inline DecodeResult huff_dec_get_code_offset(HuffDecoder *dec, Code prev)
{
    Code result;
    io_br_ensure_bits(dec->bit_reader, dec->bits);
    result = dec->code[prev.val + io_br_pop_bits(dec->bit_reader, dec->bits)];
    io_br_ensure_bits(dec->bit_reader, result.bits);
    return OK(DecodeResult, result);
}
