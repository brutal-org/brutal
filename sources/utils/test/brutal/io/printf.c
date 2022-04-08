#include <brutal/alloc.h>
#include <brutal/tests.h>
#include <brutal/text.h>

#define TEST_CASE(EXPECTED, FORMAT, ...)                                                            \
    test_case$(EXPECTED)                                                                            \
    {                                                                                               \
        expect_str_equal$(str$(EXPECTED), str_printf(test_use_alloc(), str$(FORMAT), __VA_ARGS__)); \
    }

test$(io_printf)
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

    TEST_CASE("0.000000", "%f", 0.f);
    TEST_CASE("1.000000", "%f", 1.f);
    TEST_CASE("12.000000", "%f", 12.f);
    TEST_CASE("123.000000", "%f", 123.f);

    TEST_CASE("0.000", "%.3f", 0.f);
    TEST_CASE("1.000", "%.3f", 1.f);
    TEST_CASE("12.000", "%.3f", 12.f);
    TEST_CASE("123.000", "%.3f", 123.f);

    TEST_CASE("  0.000", "%7.3f", 0.f);
    TEST_CASE("  1.000", "%7.3f", 1.f);
    TEST_CASE(" 12.000", "%7.3f", 12.f);
    TEST_CASE("123.000", "%7.3f", 123.f);

    TEST_CASE("000.000", "%07.3f", 0.f);
    TEST_CASE("001.000", "%07.3f", 1.f);
    TEST_CASE("012.000", "%07.3f", 12.f);
    TEST_CASE("123.000", "%07.3f", 123.f);

    TEST_CASE("0000000", "%07.f", 0.f);
    TEST_CASE("0000001", "%07.f", 1.f);
    TEST_CASE("0000012", "%07.f", 12.f);
    TEST_CASE("0000123", "%07.f", 123.f);
}
