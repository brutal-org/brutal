#include <cc/parse/parser.h>

CExpr cparse_expr(Lex *lex, MAYBE_UNUSED int pre, Alloc *alloc)
{
    if (cparse_skip_separator(lex, CLEX_LBRACKET))
    {
        cparse_expect_separator(lex, CLEX_RBRACKET);

        CType type = ctype_func(ctype_auto(), alloc);

        cparse_func_params(lex, &type, alloc);

        CStmt body = cparse_stmt(lex, alloc);

        return cexpr_lambda(type, body, alloc);
    }
    else
    {
        lex_throw(lex, str$("Unexpect token!"));
        return cexpr_empty();
    }
}
