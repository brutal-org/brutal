#include <brutal/alloc.h>
#include <brutal/text.h>
#include "test/test.h"

#define SIMPLE_FMT_TEST(FORMAT, ARG1, EXPECTED) \
    assert_str_equal(str$(EXPECTED), str_fmt$(test_alloc(), str$(FORMAT), ARG1))

TEST(str_fmt)
{
    Str formated = str_fmt$(alloc_global(), str$("hello world"));

    assert_str_equal(str$("hello world"), formated);
}

TEST(str_fmt_with_no_arg)
{
    Str formated = str_fmt$(alloc_global(), "hello {}");

    assert_str_equal(str$("hello {}"), formated);
}

TEST(str_fmt_with_double_bracket)
{
    Str formated = str_fmt$(alloc_global(), "hello {{}}");

    assert_str_equal(str$("hello {}"), formated);
}

TEST(str_fmt_with_str)
{
    Str formated = str_fmt$(alloc_global(), "{} this cruel {}", "hello", str$("world"));

    assert_str_equal(str$("hello this cruel world"), formated);
}

TEST(str_fmt_with_unsigned_number)
{
    SIMPLE_FMT_TEST("{}", (unsigned int)42, "42");
}

TEST(str_fmt_with_signed_number)
{
    SIMPLE_FMT_TEST("{}", (int)-42, "-42");
}

TEST(str_fmt_with_char)
{
    SIMPLE_FMT_TEST("{c}", (char)'a', "a");
}

TEST(str_fmt_with_pointer)
{
    SIMPLE_FMT_TEST("{}", (void *)1234567890, "1234567890");
}

TEST(str_fmt_binary)
{
    SIMPLE_FMT_TEST("{b}", 0b101010111011, "101010111011");
}

TEST(str_fmt_binary_prefix)
{
    SIMPLE_FMT_TEST("{#b}", 0b101010111011, "0b101010111011");
}

TEST(str_fmt_hex)
{
    SIMPLE_FMT_TEST("{x}", 0xabcdef0123456789, "abcdef0123456789");
}

TEST(str_fmt_hex_prefix)
{
    SIMPLE_FMT_TEST("{#x}", 0xabcdef0123456789, "0xabcdef0123456789");
}

TEST(str_fmt_octal)
{
    SIMPLE_FMT_TEST("{o}", 01234567, "1234567");
}

TEST(str_fmt_octal_prefix)
{
    SIMPLE_FMT_TEST("{#o}", 01234567, "0o1234567");
}

TEST(str_fmt_min_width)
{
    SIMPLE_FMT_TEST("{4}", 5, "   5");
    SIMPLE_FMT_TEST("{4}", 50000, "50000");
}

TEST(str_fmt_min_width_with_zero)
{
    SIMPLE_FMT_TEST("{04}", 5, "0005");
    SIMPLE_FMT_TEST("{04}", 50000, "50000");
    SIMPLE_FMT_TEST("{x#04}", 0x5, "0x0005");
    SIMPLE_FMT_TEST("{x#04}", 0x50000, "0x50000");
}
