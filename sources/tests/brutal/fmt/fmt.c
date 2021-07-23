#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include "tests/test.h"

#define FMT_TEST_BUFFER_INIT()                        \
    Buffer target_buffer;                             \
    buffer_init(&target_buffer, 512, alloc_global()); \
    auto writer = io_buffer_write(&target_buffer)

#define FMT_TEST_WRITER() (writer.base)

#define FMT_TEST_BUFFER_DEINIT() \
    buffer_deinit(&target_buffer);

#define FMT_TEST_STR() (str_cast_n(target_buffer.used, (char *)target_buffer.data))

TEST(fmt_no_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "hello world");

    assert_str_equal(str_cast("hello world"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "string: {}", str_cast("brutal"));

    assert_str_equal(str_cast("string: brutal"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_char_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "character: {c}", (char)'u');

    assert_str_equal(str_cast("character: u"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_positive_int_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "positive int: {}", 102030);

    assert_str_equal(str_cast("positive int: 102030"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_negative_int_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "negative int: {}", -102030);

    assert_str_equal(str_cast("negative int: -102030"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_binary_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "binary: {b}", 107247);

    assert_str_equal(str_cast("binary: 11010001011101111"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_octal_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "octal: {o}", 3574);

    assert_str_equal(str_cast("octal: 6766"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_hex_format)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "hex: {x}", 63135);

    assert_str_equal(str_cast("hex: f69f"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_prefix)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "hex: {#}", 26992);

    assert_str_equal(str_cast("hex: 0026992"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_prefix_hex)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "hex: {#x}", 63135);

    assert_str_equal(str_cast("hex: 0xf69f"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_prefix_octal)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "octal: {#o}", 3574);

    assert_str_equal(str_cast("octal: 0o6766"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_prefix_binary)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "binary: {#b}", 107247);

    assert_str_equal(str_cast("binary: 0b11010001011101111"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_upper_case)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "upper case string: {U}", str_cast("brUwUtal0"));

    assert_str_equal(str_cast("upper case string: BRUWUTAL0"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_lower_case)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "lower case string: {u}", str_cast("BRuWuTAL0"));

    assert_str_equal(str_cast("lower case string: bruwutal0"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_snake_case)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "snake_case string: {snake_case}", str_cast("HelloWorld owo"));

    assert_str_equal(str_cast("snake_case string: hello_world_owo"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_camel_case)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "CamelCase string: {CamelCase}", str_cast("hello_world owo"));

    assert_str_equal(str_cast("CamelCase string: HelloWorldOwo"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_upper_snake_case)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "snake_case upper case string: {Usnake_case}", str_cast("HelloWorld owo"));

    assert_str_equal(str_cast("snake_case upper case string: HELLO_WORLD_OWO"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}

TEST(fmt_str_lower_camel_case)
{
    FMT_TEST_BUFFER_INIT();

    print(&FMT_TEST_WRITER(), "CamelCase string: {uCamelCase}", str_cast("hello_world owo"));

    assert_str_equal(str_cast("CamelCase string: helloworldowo"), FMT_TEST_STR());

    FMT_TEST_BUFFER_DEINIT();
}
