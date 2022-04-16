#include <brutal-debug>
#include <brutal-test>
#include <cc/lex.h>
#include <cc/parse.h>

void cproc_test_case(char const *expected, char const *input)
{
    Scan scan = {};
    scan_init(&scan, str$(input));
    Lex lexed_input = clex(&scan, test_use_alloc());
    expect$(!scan.has_error);

    Lex prepoc_output = cproc_file(&lexed_input, str$("test.c"), test_use_alloc());
    expect$(!lexed_input.has_error);

    Buf buf = lex_collect(&prepoc_output, test_use_alloc());
    expect_str_equal$(buf_str(&buf), str$(expected));
}

void cproc_failling_test_case(char const *input)
{
    Scan scan = {};
    scan_init(&scan, str$(input));
    Lex lexed_input = clex(&scan, test_use_alloc());
    expect$(!scan.has_error);

    cproc_file(&lexed_input, str$("test.c"), test_use_alloc());
    expect$(lexed_input.has_error);
}

test$(cproc_simple)
{
    cproc_test_case("", "");
    cproc_test_case("abc", "abc");
    cproc_test_case("123", "123");
}

test$(cproc_stringizing, TEST_DISABLED)
{
    cproc_test_case("", "#\n#   \n#  \n");
    cproc_test_case("char const* v = \"abc\"\n", "char const* v = #abc\n");

    cproc_failling_test_case("#\n#   \"abc\"\n#  \n");
    cproc_failling_test_case("  #\n #   \n     #  \n");
}

test$(cproc_concat, TEST_DISABLED)
{
    cproc_test_case("ab", "a##b");
    cproc_failling_test_case("-##>");
}

test$(cproc_macros, TEST_DISABLED)
{
    cproc_test_case("", "#define foo bar\n");
    cproc_test_case("bar", "#define foo bar\nfoo\n");
    cproc_test_case("b", "#define c b\n#define b a\n#define a z\nc");
}

test$(cproc_function_like_macros)
{
    cproc_test_case("", "#define add(a, b) a + b\n");
    cproc_test_case("bazz", "#define foo(bar) bar\nfoo(bazz)\n");
}
