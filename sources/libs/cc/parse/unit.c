#include <cc/parse/parser.h>

CUnit cparse_unit(Lex *lex, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cparse_eat_whitespace(lex);

    while (!lex_ended(lex))
    {
        cunit_member(&unit, cunit_decl(cparse_decl(lex, alloc)));
        cparse_eat_whitespace(lex);
        lex_skip_type(lex, CLEX_SEMICOLON);
        cparse_eat_whitespace(lex);
    }

    return unit;
}
