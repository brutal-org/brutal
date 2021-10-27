#include <cc/parse/parser.h>

void cparse_eat_whitespace(Lex *lex)
{
    while (lex_curr_type(lex) == CLEX_COMMENT ||
           lex_curr_type(lex) == CLEX_WHITESPACE)
    {
        lex_next(lex);
    }
}

bool cparse_separator(Lex* lex, LexemeType type)
{
    cparse_eat_whitespace(lex);
    bool result = lex_expect(lex, type);
    cparse_eat_whitespace(lex);
    return result;
}
