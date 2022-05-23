#include <cc/parse/parser.h>

CUnit cparse_unit(Lex *lex, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cparse_whitespace(lex);

    while (!lex_ended(lex))
    {
        CDecl decl = cparse_decl(lex, &unit, alloc);
        cunit_decl(&unit, decl);

        if (decl.type != CDECL_FUNC)
        {
            cparse_expect_separator(lex, CLEX_SEMICOLON);
        }
    }

    return unit;
}
