#include <brutal-test>
#include <codec-deflate>

test$(inflate_uncompressed)
{
    uint8_t out_storage[512];
    const uint8_t in_storage[] = {1, 11, 0, 244, 255, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    // A single uncompressed block has 5 extra bytes
    expect_equal$(size, sizeof(in_storage) - 5);
}

test$(inflate_empty)
{
    uint8_t out_storage[512];
    /* Empty buffer, fixed, 6 bits of padding in the second byte set to 1 */
    const uint8_t in_storage[] = {
        0x03, 0xFC};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    expect_equal$(size, 0u);
}

test$(inflate_empty_no_literals)
{
    uint8_t out_storage[512];

    /* Empty buffer, dynamic with 256 as only literal/length code
     *
     * You could argue that since the RFC only has an exception allowing
     * one symbol for the distance tree, the literal/length tree should
     * be complete. However gzip allows this.
     *
     * See also: https://github.com/madler/zlib/issues/75
     */
    static const uint8_t in_storage[] = {
        0x05, 0xCA, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xFF,
        0x6B, 0x01, 0x00};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    expect_equal$(size, 0u);
}

test$(inflate_rle)
{
    uint8_t out_storage[512];
    /* 256 zero bytes compressed using RLE (only one distance code) */
    const uint8_t in_storage[] = {
        0xE5, 0xC0, 0x81, 0x00, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xFC,
        0xA9, 0x07, 0x39, 0x73, 0x01};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    // Should be 256
    expect_equal$(size, 256u);
}

test$(inflate_huffman)
{
    uint8_t out_storage[512];
    /* 256 zero bytes compressed using Huffman only (no match or distance codes) */
    const uint8_t in_storage[] = {
        0x05, 0xCA, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xFF,
        0xD5, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    // Should be 256
    expect_equal$(size, 256u);
}

test$(inflate_max_matchlen)
{
    uint8_t out_storage[512];
    /* 259 zero bytes compressed using literal/length code 285 (len 258) */
    static const uint8_t in_storage[] = {
        0xED, 0xCC, 0x81, 0x00, 0x00, 0x00, 0x00, 0x80, 0xA0, 0xFC,
        0xA9, 0x17, 0xB9, 0x00, 0x2C};

    size_t size = UNWRAP(deflate_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    // Should be 259
    expect_equal$(size, 259u);
}
