#include <brutal/alloc.h>
#include <brutal/text.h>
#include "test/test.h"

#define TEST_CASE(EXPECTED, FORMAT, ...) \
    assert_str_equal(str$(EXPECTED), str_printf(test_alloc(), str$(FORMAT), __VA_ARGS__))

TEST(io_printf)
{
    TEST_CASE("hello world", "hello world", str$("world"));
    TEST_CASE("hello world", "hello %s", "world");

    TEST_CASE("the answer is 42", "the answer is %d", 42);
    TEST_CASE("the answer is 42", "the answer is %u", 42u);
    TEST_CASE("the answer is -42", "the answer is %d", -42);

    TEST_CASE("magic number: 0x12345678", "magic number: 0x%x", 0x12345678);
}
