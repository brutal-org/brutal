#include <cc/parse/parser.h>

/*
CExpr cparse_expr_prefix(Lex *lex, Alloc *alloc)
{
}

CExpr cparse_expr_infix(Lex *lex, CExpr lhs, Alloc *alloc)
{
}

*/

CExpr cparse_expr(Lex *lex, int pre, Alloc *alloc)
{
    UNUSED(lex);
    UNUSED(pre);
    UNUSED(alloc);

    return cexpr_empty();
}
