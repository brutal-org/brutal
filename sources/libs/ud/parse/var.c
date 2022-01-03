#include <ud/parse/decl.h>
#include <ud/parse/lexer.h>

UdDecl ud_parse_var_decl(Lex *lex, Alloc *alloc)
{
    UdDecl ret = {};

    ret.type = UD_DECL_VAR;

    ud_parse_whitespace(lex);

    if (lex_curr(lex).type == UDLEX_IDENT)
    {
        ret.name = str_dup(lex_curr(lex).str, alloc);

        lex_next(lex);
    }

    ud_parse_whitespace(lex);

    if (lex_expect(lex, UDLEX_COLON) && lex_curr(lex).type != UDLEX_EQUAL)
    {
        ud_parse_whitespace(lex);

        ud_expect(lex, UDLEX_IDENT);

        ret.var.type.type = ud_str_to_type(lex_peek(lex, -1).str);

        ret.var.type.name = str_dup(lex_peek(lex, -1).str, alloc);

        ud_expect(lex, UDLEX_WHITESPACE);
    }

    else
    {
        ret.var.type.name = str$("inferred");
    }

    ud_parse_whitespace(lex);

    if (lex_curr(lex).type == UDLEX_COLON || lex_peek(lex, -1).type == UDLEX_COLON)
    {
        ret.var.mutable = true;

        if (lex_curr(lex).type == UDLEX_COLON)
        {
            lex_next(lex);
        }
    }

    ud_expect(lex, UDLEX_EQUAL);

    ud_parse_whitespace(lex);

    ret.var.value = alloc_malloc(alloc, sizeof(UdExpr));

    UdExpr out = ud_parse_expr(lex, alloc).expr;

    mem_cpy(ret.var.value, &out, sizeof(UdExpr));

    lex_next(lex);

    ud_parse_whitespace(lex);
    ud_expect(lex, UDLEX_SEMICOLON);

    return ret;
}
