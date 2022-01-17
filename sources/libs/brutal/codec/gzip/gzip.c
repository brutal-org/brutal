#include <brutal/codec/deflate/inflate.h>
#include <brutal/codec/errors.h>
#include <brutal/codec/gzip/gzip.h>
#include <brutal/hash/adler32.h>
#include <brutal/io/mem_view.h>

typedef enum
{
    FLAG_TEXT = 1,
    FLAG_HCRC = 2,
    FLAG_EXTRA = 4,
    FLAG_NAME = 8,
    FLAG_COMMENT = 16
} GZipFlag;

IoResult gzip_decompress_data(const uint8_t *in, size_t in_len, const uint8_t *out, size_t out_len)
{
    // Input
    MemView in_view;
    mem_view_init(&in_view, in_len, in);
    IoReader reader = mem_view_reader(&in_view);

    // Output
    MemView out_view;
    mem_view_init(&out_view, out_len, out);
    IoWriter writer = mem_view_writer(&out_view);

    return gzip_decompress_stream(writer, reader);
}

IoResult gzip_decompress_stream(IoWriter writer, IoReader reader)
{
    uint8_t id1, id2;
    // Read CMF (compression method & flags)
    TRY(IoResult, io_read(reader, &id1, 1));
    // Flags
    TRY(IoResult, io_read(reader, &id2, 1));

    //
    if (id1 != 0x1f || id2 != 0x8b)
    {
        return ERR(IoResult, ERR_INVALID_DATA);
    }

    uint8_t cm;
    TRY(IoResult, io_read(reader, &cm, 1));

    // Check method is deflate
    if (cm != 8)
    {
        return ERR(IoResult, ERR_NOT_IMPLEMENTED);
    }

    uint8_t flags;
    TRY(IoResult, io_read(reader, &flags, 1));

    // Check that reserved bits are zero
    if (flags & 0xE0)
    {
        return ERR(IoResult, ERR_INVALID_DATA);
    }

    // Skip unused bytes
    TRY(IoResult, io_skip(reader, 6));

    // Skip extra data if present
	if (flags & FLAG_EXTRA) {
		le_uint16_t value;
        TRY(IoResult, io_read(reader, (uint8_t*)&value, 2));
        uint16_t xlen = load_le(value);
        TRY(IoResult, io_skip(reader, xlen));
	}

    // Skip file name if present
    if (flags & FLAG_NAME)
    {
        uint8_t val = 0;
        do
        {
            TRY(IoResult, io_read(reader, &val, 1));
        } while (val != 0);
    }

    // Skip file comment if present
    if (flags & FLAG_COMMENT)
    {
        uint8_t val = 0;
        do
        {
            TRY(IoResult, io_read(reader, &val, 1));
        } while (val != 0);
    }

	// Check header crc if present
	if (flags & FLAG_HCRC) {
		le_uint16_t value;
        TRY(IoResult, io_read(reader, (uint8_t*)&value, 2));
        uint16_t hcrc = load_le(value);
        UNUSED(hcrc);
        // TODO: Header CRC32C
	}

    // TODO: Data CRC32C
    size_t decompressed = TRY(IoResult, deflate_decompress_stream(writer, reader));

    return OK(IoResult, decompressed);
}
