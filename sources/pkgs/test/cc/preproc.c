#include <brutal/base.h>
#include <cc/parse.h>
#include <cc/proc/proc.h>
#include "test/cc/cc.h"
#include "test/test.h"

TEST(empty_preproc_test)
{
    Str from = str$("");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$(""));
}

TEST(null_preprocessing_directive)
{
    Str from = str$("#\n#   \n#  \n");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$(""));
}

TEST_WITH_FLAGS(directive_invalid_position, TEST_EXPECTED_TO_FAIL)
{
    Str from = str$("  #\n"
                    " #   \n"
                    "     #  \n");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$(""));
}

TEST(macro_parsing)
{
    Str from = str$("#define mac owo\n");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$(""));
}

TEST(macro_parsing_args)
{
    Str from = str$("#define mac(owo, urrs) owo + urrs\n");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$(""));
}

TEST(macro_gen)
{
    Str from = str$("#define mac owo\n"
                    "mac");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("owo"));
}

TEST(macro_gen_with_arg)
{
    Str from = str$("#define mac(owo) owo\n"
                    "mac(hello)");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("hello"));
}

TEST(macro_gen_rec)
{
    Str from = str$("#define testa testb\n"
                    "#define testb testa\n"
                    "#define testc testb\n"
                    "testc");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("testb"));
}

TEST(concatenation)
{
    Str from = str$("#define testa(aa, bb) aa ## bb\n"
                    "testa(10,10)");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("1010"));
}

TEST(concatenation_rec)
{
    Str from = str$("#define testb(aa, bb) aa ## bb\n"
                    "#define testa(aa, bb) aa ## testb(bb, aa)\n"
                    "testa(10,20)");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("102010"));
}

TEST(concatenation_in_one_token)
{
    Str from = str$("#define testa(aa, bb) aa ## bb\n"
                    "testa(-,>)");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("->"));
}

TEST_WITH_FLAGS(concatenation_should_form_one_token, TEST_EXPECTED_TO_FAIL)
{
    Str from = str$("#define testa(aa, bb) aa ## bb\n"
                    "testa(@,>)");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("@>"));
}

TEST(concatenation_hell)
{
    Str from = str$("#define testa(aa, bb) aa #### bb\n"
                    "testa(-,>)");
    ctx_lex_proc(processed, from);

    assert_lex_eq(&processed, str$("->"));
}
