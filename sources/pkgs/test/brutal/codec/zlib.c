#include <brutal/alloc/global.h>
#include <brutal/codec/zlib/zlib.h>
#include <brutal/io/bit_write.h>
#include <brutal/io/mem_view.h>
#include "test/test.h"

TEST(zlib_empty_raw)
{
    uint8_t out_storage[512];
    // Empty buffer, uncompressed
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 0);
}

TEST(zlib_empty_fixed)
{
    uint8_t out_storage[512];
    // Empty buffer, fixed Huffman
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 0);
}

TEST(zlib_empty_dynamic)
{
    uint8_t out_storage[512];
    // Empty buffer, dynamic Huffman
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x05, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0xFF, 0xD5, 0x08, 0x00, 0x00, 0x00, 0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 0);
}

TEST(zlib_onebyte_raw)
{
    uint8_t out_storage[512];
    // One byte 00, uncompressed
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x01, 0x01, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x01,
        0x00, 0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 1);
}

TEST(zlib_onebyte_fixed)
{
    uint8_t out_storage[512];
    // One byte 00, fixed Huffman
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x63, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 1);
}

TEST(zlib_onebyte_dynamic)
{
    uint8_t out_storage[512];
    // One byte 00, dynamic Huffman
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x05, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0xFF, 0xD5, 0x10, 0x00, 0x01, 0x00, 0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 1);
}

TEST(zlib_zeroes)
{
    uint8_t out_storage[512];
    // 256 zero bytes, to test unrolling in Adler-32
    const uint8_t in_storage[] = {
        0x78, 0x9C, 0x63, 0x60, 0x18, 0xD9, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x01};

    size_t size = UNWRAP(zlib_decompress_data(in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    assert_equal(size, 256);
}
