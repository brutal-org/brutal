#include <brutal/alloc.h>
#include <brutal/text.h>
#include "test/test.h"

#define TEST_CASE(EXPECTED, FORMAT, ...)                                                     \
    if (test_case_begin(str$(EXPECTED)))                                                     \
    {                                                                                        \
        assert_str_equal(str$(EXPECTED), str_fmt$(test_alloc(), str$(FORMAT), __VA_ARGS__)); \
        test_case_end();                                                                     \
    }

TEST(io_fmt)
{
    TEST_CASE("hello, world", "hello, world");
    TEST_CASE("hello, world", "hello, {}", "world");
    TEST_CASE("hello, {}", "hello, {}");
    TEST_CASE("hello, {}", "hello, {{}}");
    TEST_CASE("hello, {} 1234", "hello, {{}} {}", 1234);

    TEST_CASE("hello, world", "{}, {}", "hello", str$("world"));

    TEST_CASE("the answer is 42", "the answer is {}", 42);
    TEST_CASE("the answer is -42", "the answer is {}", -42);

    TEST_CASE("a", "{c}", 'a');
    TEST_CASE("97", "{}", 'a');

    TEST_CASE("1234567890", "{}", (void *)1234567890);
    TEST_CASE("101010111011", "{b}", 0b101010111011);
    TEST_CASE("0b101010111011", "{#b}", 0b101010111011);
    TEST_CASE("abcdef0123456789", "{x}", 0xabcdef0123456789);
    TEST_CASE("0xabcdef0123456789", "{#x}", 0xabcdef0123456789);
    TEST_CASE("1234567", "{o}", 01234567);
    TEST_CASE("0o1234567", "{#o}", 01234567);
    TEST_CASE("   5", "{4}", 5);
    TEST_CASE("50000", "{4}", 50000);
    TEST_CASE("0005", "{04}", 5);
    TEST_CASE("50000", "{04}", 50000);
    TEST_CASE("0x0005", "{x#04}", 0x5);
    TEST_CASE("0x50000", "{x#04}", 0x50000);
}
