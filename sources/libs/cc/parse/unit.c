#include <cc/parse/parser.h>

CUnit cparse_unit(Lex *lex, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cparse_whitespace(lex);

    while (!lex_ended(lex))
    {
        cunit_member(&unit, cunit_decl(cparse_decl(lex, alloc)));

        cparse_separator(lex, CLEX_SEMICOLON);
    }

    return unit;
}
