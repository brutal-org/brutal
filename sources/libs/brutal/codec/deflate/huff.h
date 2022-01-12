#pragma once
#include <brutal/io/bit_read.h>

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

DecodeResult huff_dec_get_code(HuffDecoder *dec)
{
    Code result;
    for (;;)
    {
        io_br_ensure_bits(dec->bit_reader, dec->bits);
        result = dec->code[io_br_pop_bits(dec->bit_reader, dec->bits)];
        if ((uint32_t)(result.bits) <= dec->bit_reader->bitcount)
            break;
        TRY(DecodeResult, io_br_get_byte(dec->bit_reader));
    }

    return OK(DecodeResult, result);
}

DecodeResult huff_dec_get_code_offset(HuffDecoder *dec, Code prev)
{
    Code result;
    for (;;)
    {
        result = dec->code[prev.val + io_br_pop_bits(dec->bit_reader, dec->bits)];
        if ((uint32_t)(result.bits) <= dec->bit_reader->bitcount)
            break;
        TRY(DecodeResult, io_br_get_byte(dec->bit_reader));
    }

    return OK(DecodeResult, result);
}
