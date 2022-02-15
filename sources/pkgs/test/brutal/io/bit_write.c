#include <brutal/alloc.h>
#include <brutal/io/wbits.h>
#include "test/test.h"

TEST(bitwriter_write_aligned)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);
    Buf buf;
    buf_init(&buf, 512, base$(&heap));
    IoWriter writer = buf_writer(&buf);
    // Initialize our bitwriter
    IoWBits bit_writer;
    io_wbits_init(&bit_writer, writer);
    assert_equal(buf.used, 0u);
    // Write 8 bits
    unsigned data = 0b01010101;
    io_wbits_add_bits(&bit_writer, data, 8);
    io_wbits_flush_bits(&bit_writer);
    assert_equal(buf.used, 1u);
    // Write 16 bits
    data = 0b0101010101010101;
    io_wbits_add_bits(&bit_writer, data, 16);
    io_wbits_flush_bits(&bit_writer);
    assert_equal(buf.used, 3u);
}
