#include <cc/parse/parser.h>

CUnit cparse_unit(Lex *lex, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cparse_whitespace(lex);

    while (!lex_ended(lex))
    {
        cunit_decl(&unit, cparse_decl(lex,  &unit, alloc));
        cparse_expect_separator(lex, CLEX_SEMICOLON);
    }

    return unit;
}
