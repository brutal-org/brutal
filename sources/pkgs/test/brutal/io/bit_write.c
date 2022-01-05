#include <brutal/alloc.h>
#include <brutal/io/bit_write.h>
#include "test/test.h"

TEST(bitwriter_write_aligned)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);
    Buf buf;
    buf_init(&buf, 512, base$(&heap));
    IoWriter writer = buf_writer(&buf);
    BitWriter bit_writer;
    io_bw_init(&bit_writer, &writer);
    assert_equal(buf.used, 0);
    unsigned data = 0b01010101;
    io_bw_add_bits(&bit_writer, 8, data);
    io_bw_flush_bits(&bit_writer);
    assert_equal(buf.used, 1);
}
