#include <ud/parse/decl.h>
#include <ud/parse/lexer.h>

UdAstNode ud_parse_decl(Lex *lex, Alloc *alloc)
{
    UdAstNode ret = {};

    ret.expr.type = UD_EXPR_DECL;
    ret.expr.decl.type = UD_DECL_NONE;

    if (lex_expect(lex, UDLEX_LET))
    {
        ret.expr.decl = ud_parse_var_decl(lex, alloc);
    }

    else if (lex_expect(lex, UDLEX_FUNC))
    {
        ret.expr.decl = ud_parse_func_decl(lex, alloc);
    }

    return ret;
}
