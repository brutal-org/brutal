#include <cc/parse/parser.h>

void cparse_whitespace(Lex *lex)
{
    while (lex_curr_type(lex) == CLEX_COMMENT ||
           lex_curr_type(lex) == CLEX_WHITESPACE)
    {
        lex_next(lex);
    }
}

bool cparse_separator(Lex *lex, LexemeType type)
{
    cparse_whitespace(lex);
    bool result = lex_expect(lex, type);
    cparse_whitespace(lex);
    return result;
}
