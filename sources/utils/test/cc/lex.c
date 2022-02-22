#include "test/cc/cc.h"
#include "test/test.h"

bool assert_lex_eq(Lex *first, Str left)
{
    int idx = 0;

    vec_foreach(lexeme, &first->lexemes)
    {
        if ((size_t)idx > left.len)
        {
            panic$("expected len eq {} != {}", idx, left.len);
        }

        Str part = str_sub(left, idx, idx + lexeme->str.len);

        if (!str_eq(part, lexeme->str))
        {
            panic$("tok '{}' != '{}'", lexeme->str, part);
        }

        idx += lexeme->str.len;
    }

    if ((size_t)idx != left.len)
    {
        panic$("expected len eq {} != {}", left.len, idx);
    }

    return true;
}

TEST(lex_eq_test)
{
    Str from = str$("int these_are_some_tokens >= + 10 \"efeifjieij\" \n");

    ctx_lex(lexer, from);

    assert_truth(assert_lex_eq(&lexer, from));
}

TEST_WITH_FLAGS(lex_eq_error_test, TEST_EXPECTED_TO_FAIL)
{

    Str from = str$("° <- this should not be valid");

    ctx_lex(lexer, from);

    (void)lexer;
}
