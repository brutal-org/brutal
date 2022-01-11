#include <brutal/alloc/global.h>
#include <brutal/codec/deflate/deflate.h>
#include <brutal/codec/deflate/inflate.h>
#include <brutal/io/bit_write.h>
#include <brutal/io/mem_view.h>
#include "test/test.h"

TEST(deflate_uncompressed)
{
    uint8_t out_storage[512];
    const uint8_t in_storage[] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    DeflateCompressionContext ctx;
    deflate_init(&ctx, 0, alloc_global());

    size_t size = UNWRAP(deflate_compress_data(&ctx, in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    deflate_deinit(&ctx);
    // A single uncompressed block has 5 extra bytes
    assert_equal(size, sizeof(in_storage) + 5);
}

TEST(inflate_uncompressed)
{
    uint8_t out_storage[512];
    const uint8_t in_storage[] = {1, 11, 0, 244, 255, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    // A single uncompressed block has 5 extra bytes
    assert_equal(size, sizeof(in_storage) - 5);
}

TEST(inflate_huffman)
{
    uint8_t out_storage[512];
    /* 256 zero bytes compressed using Huffman only (no match or distance codes) */
    const uint8_t in_storage[] = {
        0x05, 0xCA, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xFF,
        0xD5, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02};

    UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    // A single uncompressed block has 5 extra bytes
    // assert_equal(size, sizeof(in_storage) - 5);
}
