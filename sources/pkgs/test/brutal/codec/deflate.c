#include <brutal/alloc/global.h>
#include <brutal/codec/deflate/deflate.h>
#include <brutal/io/bit_write.h>
#include <brutal/io/mem_view.h>
#include "test/test.h"

TEST(deflate_uncompressed)
{
    uint8_t out_storage[512];
    uint8_t in_storage[] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    DeflateCompressionContext ctx;
    deflate_init(&ctx, 0, alloc_global());

    size_t size = UNWRAP(deflate_compress_data(&ctx, in_storage, sizeof(in_storage), out_storage, sizeof(out_storage)));

    deflate_deinit(&ctx);
    // A single uncompressed block has 5 extra bytes
    assert_equal(size, sizeof(in_storage) + 5);
}
