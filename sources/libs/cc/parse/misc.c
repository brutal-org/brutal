#include <cc/parse/parser.h>

void cparse_eat_whitespace(Lex *lex)
{
    while (lex_curr_type(lex) == CLEX_COMMENT ||
           lex_curr_type(lex) == CLEX_WHITESPACE)
    {
        lex_next(lex);
    }
}
