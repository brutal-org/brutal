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

TEST(bid_c_conversion_test_method)
{

    Str expected = str_cast("#pragma once\n"
                            "\n"
                            "#include <brutal/base.h>\n"
                            "\n"
                            "// -- generated idl file --\n"
                            "\n"
                            "// interface: net\n"
                            "\n"
                            "enum net_messages_type\n"
                            "{\n"
                            "\tnet_msg_uwu,\n"
                            "};\n"
                            "\n"
                            "// - net.uwu\n"
                            "\n"
                            "struct net_uwu_send\n"
                            "{\n"
                            "\tint x;\n"
                            "};\n"
                            "\n"
                            "int net_uwu(BrTask target, struct net_uwu_send* send);\n\n");
    Buffer target;
    buffer_init(&target, expected.len + 2, alloc_global());
    auto writer = io_buffer_write(&target);

    auto bid = UNWRAP(init_bid(str_cast(" interface net { method uwu(x: int) -> int;}")));
    convert_bid_to_c(&bid, &writer.base);

    assert_str_equal(str_cast_n(target.used, (char *)target.data), expected);

    buffer_deinit(&target);
    destroy_bid(&bid);
};

TEST(bid_c_conversion_test_errors)
{

    Str expected = str_cast("#pragma once\n"
                            "\n"
                            "#include <brutal/base.h>\n"
                            "\n"
                            "// -- generated idl file --\n"
                            "\n"
                            "// interface: net\n"
                            "\n"
                            "enum net_messages_type\n"
                            "{\n"
                            "};\n"
                            "\n"
                            "enum net_errors_type\n"
                            "{\n"
                            "\tnet_no_error,\n"
                            "\tnet_OWO,\n"
                            "\tnet_devse_is_life,\n"
                            "};\n"
                            "\n"
                            "struct net_errors\n"
                            "{\n"
                            "\tenum net_errors_type type;\n"
                            "};\n"
                            "\n");
    Buffer target;
    buffer_init(&target, expected.len + 2, alloc_global());
    auto writer = io_buffer_write(&target);

    auto bid = UNWRAP(init_bid(str_cast(" interface net { errors {OWO, devse_is_life};}")));
    convert_bid_to_c(&bid, &writer.base);

    assert_str_equal(str_cast_n(target.used, (char *)target.data), expected);

    buffer_deinit(&target);
    destroy_bid(&bid);
};

TEST(bid_c_conversion_type)
{

    Str expected = str_cast("#pragma once\n"
                            "\n"
                            "#include <brutal/base.h>\n"
                            "\n"
                            "// -- generated idl file --\n"
                            "\n"
                            "// interface: net\n"
                            "\n"
                            "enum net_messages_type\n"
                            "{\n"
                            "};\n"
                            "\n"
                            "typedef int Handle;\n\n");
    Buffer target;
    buffer_init(&target, expected.len + 2, alloc_global());
    auto writer = io_buffer_write(&target);

    auto bid = UNWRAP(init_bid(str_cast(" interface net { type Handle: int;}")));
    convert_bid_to_c(&bid, &writer.base);

    assert_str_equal(str_cast_n(target.used, (char *)target.data), expected);

    buffer_deinit(&target);
    destroy_bid(&bid);
};
