#include <cc/parse/parser.h>

void cparse_whitespace(Lex *lex)
{
    while (lex_curr_type(lex) == CLEX_WHITESPACE)
    {
        lex_next(lex);
    }
}

bool cparse_expect_separator(Lex *lex, LexemeType type)
{
    cparse_whitespace(lex);
    bool result = lex_expect(lex, type);
    cparse_whitespace(lex);
    return result;
}

bool cparse_skip_separator(Lex *lex, LexemeType type)
{
    cparse_whitespace(lex);
    bool result = lex_skip_type(lex, type);
    cparse_whitespace(lex);
    return result;
}

bool cparse_is_separator(Lex *lex, LexemeType type)
{
    cparse_whitespace(lex);
    return lex_curr_type(lex) == type;
}

Lexeme cparse_peek_separator(Lex *lex, int offset)
{
    int off = 0;
    while (lex_peek_type(lex, off) == CLEX_WHITESPACE && off != offset && !lex_ended(lex))
    {
        off++;
    }

    return lex_peek(lex, off);
}
