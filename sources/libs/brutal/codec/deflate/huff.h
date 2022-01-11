#pragma once
#include <brutal/io/bit_read.h>

typedef struct
{
    unsigned char op;   /* operation, extra bits, table bits */
    unsigned char bits; /* bits in this part of the code */
    unsigned short val; /* offset in table or code value */
} Code;

typedef Result(Error, Code) DecodeResult;

typedef struct
{
    BitReader *bit_reader;
    const Code *code;
    unsigned bits;
} HuffDecoder;

void huff_dec_init(HuffDecoder *dec, BitReader *bit_reader, const Code *code, unsigned bits)
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
        result = dec->code[io_br_pop_bits(dec->bit_reader, dec->bits)];
        if ((unsigned)(result.bits) <= dec->bit_reader->bitcount)
            break;
        TRY(DecodeResult, io_br_get_byte(dec->bit_reader));
    }

    return OK(DecodeResult, result);
}
