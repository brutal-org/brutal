#include <brutal-test>
#include <codec-deflate>

test$(deflate_uncompressed)
{
    uint8_t out_storage[512];
    const uint8_t in_storage[] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    DeflateCompressor ctx;
    deflate_init(&ctx, 0, test_use_alloc());

    size_t size = UNWRAP(deflate_compress_data(&ctx, in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    deflate_deinit(&ctx);
    // A single uncompressed block has 5 extra bytes
    expect_equal$(size, sizeof(in_storage) + 5);
}
