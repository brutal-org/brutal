#include <brutal-fmt>
#include <brutal-test>

#define TEST_CASE(EXPECTED, FORMAT, ...)                                                             \
    test_case$(EXPECTED)                                                                             \
    {                                                                                                \
        expect_str_equal$(str$(EXPECTED), fmt_sprintf(test_use_alloc(), str$(FORMAT), __VA_ARGS__)); \
    }

test$(fmt_printf)
{
    TEST_CASE("hello world", "hello world", str$("world"));
    TEST_CASE("hello world", "hello %s", "world");

    TEST_CASE("the answer is 42", "the answer is %d", 42);
    TEST_CASE("the answer is 42", "the answer is %u", 42u);
    TEST_CASE("the answer is -42", "the answer is %d", -42);

    TEST_CASE("magic number: 0x12345678", "magic number: 0x%x", 0x12345678);

    TEST_CASE("STCFN000", "STCFN%.3d", 0);
    TEST_CASE("STCFN001", "STCFN%.3d", 1);
    TEST_CASE("STCFN012", "STCFN%.3d", 12);
    TEST_CASE("STCFN123", "STCFN%.3d", 123);

    TEST_CASE("000", "%.3d", 0);
    TEST_CASE("001", "%.3d", 1);
    TEST_CASE("012", "%.3d", 12);
    TEST_CASE("123", "%.3d", 123);

    TEST_CASE("    5", "%5d", 5);
    TEST_CASE("    5", "%5u", 5);
    TEST_CASE("  000", "%5.3d", 0);
    TEST_CASE("  001", "%5.3d", 1);
    TEST_CASE("  012", "%5.3d", 12);
    TEST_CASE("  123", "%5.3d", 123);

    TEST_CASE("    0", "%5d", 0);
    TEST_CASE("    1", "%5d", 1);
    TEST_CASE("   12", "%5d", 12);
    TEST_CASE("  123", "%5d", 123);

    TEST_CASE("     ", "%5.d", 0);
    TEST_CASE("    1", "%5.d", 1);
    TEST_CASE("   12", "%5.d", 12);
    TEST_CASE("  123", "%5.d", 123);

    TEST_CASE("00000", "%05d", 0);
    TEST_CASE("00001", "%05d", 1);
    TEST_CASE("00012", "%05d", 12);
    TEST_CASE("00123", "%05d", 123);
    TEST_CASE("0x123", "%#05x", 0x123);
    TEST_CASE("0x123", "%#05x", 0x123);
    TEST_CASE("0x123", "%#010x", 0x123);
}
