#include <brutal-hash>
#include <codec-deflate>

#include "zlib.h"

IoResult zlib_decompress_data(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len)
{
    // Input
    IoMem in_view;
    io_mem_init(&in_view, in_len, in);
    IoReader reader = io_mem_reader(&in_view);

    // Output
    IoMem out_view;
    io_mem_init(&out_view, out_len, out);
    IoWriter writer = io_mem_writer(&out_view);

    return zlib_decompress_stream(writer, reader);
}

IoResult zlib_decompress_stream(IoWriter writer, IoReader reader)
{
    uint8_t cmf, flg;
    // Read CMF (compression method & flags)
    TRY(IoResult, io_read(reader, &cmf, 1));
    // Flags
    TRY(IoResult, io_read(reader, &flg, 1));

    // The FCHECK value must be such that CMF and FLG, when viewed as
    // a 16-bit unsigned integer stored in MSB order (CMF*256 + FLG),
    // is a multiple of 31.
    if ((256 * cmf + flg) % 31)
    {
        return ERR(IoResult, ERR_INVALID_DATA);
    }

    // Check method is deflate
    if ((cmf & 0x0F) != 8)
    {
        return ERR(IoResult, ERR_NOT_IMPLEMENTED);
    }

    // Check window size is valid
    uint8_t window_bits = cmf >> 4;
    if (window_bits > 7)
    {
        return ERR(IoResult, ERR_INVALID_DATA);
    }

    // Check there is no preset dictionary
    if (flg & 0x20)
    {
        return ERR(IoResult, ERR_NOT_IMPLEMENTED);
    }

    // Calculate the Adler32 checksum of the uncompressed data on the fly
    Adler32 adler;
    adler32_init(&adler, writer);
    IoWriter checksum_writer = adler32_writer(&adler);

    size_t decompressed = TRY(IoResult, deflate_decompress_stream(checksum_writer, reader));

    // Get Adler-32 checksum of original data
    be_uint32_t value;
    TRY(IoResult, io_read(reader, (uint8_t *)&value, 4));
    uint32_t adler32 = be_load$(value);

    if (adler32 != adler32_get(&adler))
    {
        return ERR(IoResult, ERR_CHECKSUM_MISMATCH);
    }

    return OK(IoResult, decompressed);
}
