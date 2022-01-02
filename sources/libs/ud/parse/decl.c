#include <brutal/debug.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>
#include "brutal/alloc/base.h"
#include "brutal/parse/lex.h"

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

    if (lex_expect(lex, UDLEX_COLON))
    {
        ud_parse_whitespace(lex);

        ud_expect(lex, UDLEX_IDENT);

        ret.var.type.type = ud_str_to_type(lex_peek(lex, -1).str);

        ret.var.type.name = str_dup(lex_peek(lex, -1).str, alloc);
    }

    else
    {
        ret.var.type.name = str$("inferred");
    }

    ud_parse_whitespace(lex);

    ud_expect(lex, UDLEX_EQUAL);

    ud_parse_whitespace(lex);

    if (lex_expect(lex, UDLEX_COLON))
    {
        ret.var.mutable = true;

        lex_next(lex);
    }

    ret.var.value = alloc_malloc(alloc, sizeof(UdExpr));

    UdExpr out = ud_parse_expr(lex, alloc).expr;

    mem_cpy(ret.var.value, &out, sizeof(UdExpr));

    lex_next(lex);

    ud_parse_whitespace(lex);
    ud_expect(lex, UDLEX_SEMICOLON);

    return ret;
}

UdDecl ud_parse_func_decl(Lex *lex, Alloc *alloc)
{
    UdDecl ret = {};

    ret.type = UD_DECL_FUNC;

    vec_init(&ret.func.params, alloc);
    vec_init(&ret.func.body, alloc);

    ud_parse_whitespace(lex);

    if (lex_curr(lex).type == UDLEX_IDENT)
    {
        ret.name = str_dup(lex_curr(lex).str, alloc);

        lex_next(lex);
    }

    ud_parse_whitespace(lex);

    ud_expect(lex, UDLEX_LPAREN);

    ud_parse_whitespace(lex);

    // foo: bar
    while (lex_curr(lex).type != UDLEX_RPAREN)
    {
        UdFuncParam param = {};

        // foo
        param.name = lex_curr(lex).str;

        ud_expect(lex, UDLEX_IDENT);

        // :
        ud_parse_whitespace(lex);

        ud_expect(lex, UDLEX_COLON);

        ud_parse_whitespace(lex);

        // baz
        param.type.name = lex_next(lex).str;

        param.type.type = ud_str_to_type(param.type.name);

        ud_parse_whitespace(lex);

        // If param is not the last one
        if (lex_peek(lex, 1).type != UDLEX_RPAREN)
        {
            ud_parse_whitespace(lex);

            if (lex_curr(lex).type != UDLEX_RPAREN)
            {
                ud_parse_whitespace(lex);

                ud_expect(lex, UDLEX_COMMA);

                ud_parse_whitespace(lex);
            }
        }

        else
        {
            ud_parse_whitespace(lex);

            lex_next(lex);

            ud_parse_whitespace(lex);
        }

        if (ud_get_error())
        {
            break;
        }

        vec_push(&ret.func.params, param);
    }

    ud_expect(lex, UDLEX_RPAREN);

    ud_parse_whitespace(lex);

    if (lex_expect(lex, UDLEX_THIN_ARROW))
    {

        ud_expect(lex, UDLEX_WHITESPACE);

        ud_parse_whitespace(lex);

        ret.func.return_type.name = lex_next(lex).str;

        ud_expect(lex, UDLEX_WHITESPACE);
    }

    else
    {
        ret.func.return_type.name = str$("inferred");
    }

    ud_parse_whitespace(lex);

    if (lex_expect(lex, UDLEX_EQUAL))
    {
        lex_next(lex);

        vec_push(&ret.func.body, ud_parse_expr(lex, alloc));

        lex_next(lex);

        ud_parse_whitespace(lex);
        ud_expect(lex, UDLEX_SEMICOLON);
    }

    else if (lex_expect(lex, UDLEX_LCBRACE))
    {
        ud_parse_whitespace(lex);

        while (lex_curr(lex).type != UDLEX_RCBRACE)
        {
            UdAst ast = ud_parse(lex, alloc);

            vec_push(&ret.func.body, ast.data[0]);

            if (lex_peek(lex, 1).type == UDLEX_WHITESPACE || lex_curr(lex).type == UDLEX_WHITESPACE)
            {
                lex_next(lex);

                ud_parse_whitespace(lex);
            }

            if (ud_get_error())
            {
                break;
            }
        }
    }

    return ret;
}

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
