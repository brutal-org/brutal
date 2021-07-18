#include <bid/bid.h>
#include <bid/c_convert/bid_convert_c.h>
#include <brutal/alloc.h>
#include <brutal/io/buffer.h>
#include "brutal/host/io.h"
#include "brutal/log/assert.h"
#include "brutal/text/str.h"
#include "tests/test.h"

TEST(bid_c_conversion_test)
{
    Str expected = str_cast("#pragma once\n"
                            "\n"
                            "#include <brutal/base.h>\n"
                            "\n"
                            "// -- generated idl file --\n"
                            "\n"
                            "// interface: net\n"
                            "\n");

    Buffer target;
    buffer_init(&target, expected.len + 2, alloc_global());
    auto writer = io_buffer_write(&target);

    auto bid = UNWRAP(init_bid(str_cast(" interface net { }")));
    convert_bid_to_c(&bid, &writer.base);

    assert_str_equal(str_cast_n(target.used, (char *)target.data), expected);

    buffer_deinit(&target);
    destroy_bid(&bid);
};
