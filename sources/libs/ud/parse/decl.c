#include <brutal/debug.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>

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

        ret.var.type.name = str_dup(lex_peek(lex, -1).str, alloc);
    }

    else
    {
        ret.var.type.name = str$("inferred");
    }

    ud_parse_whitespace(lex);

    ud_expect(lex, UDLEX_EQUAL);

    ud_parse_whitespace(lex);

    ret.var.value = ud_parse_expr(lex, alloc).expr;

    lex_next(lex);

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

        vec_push(&ret.func.params, param);
    }

    ud_expect(lex, UDLEX_RPAREN);

    ud_parse_whitespace(lex);

    ud_expect(lex, UDLEX_THIN_ARROW);

    ud_expect(lex, UDLEX_WHITESPACE);

    ud_parse_whitespace(lex);

    ret.func.return_type.name = lex_next(lex).str;

    ud_expect(lex, UDLEX_WHITESPACE);

    ud_parse_whitespace(lex);

    if (lex_expect(lex, UDLEX_EQUAL))
    {
        lex_next(lex);

        vec_push(&ret.func.body, ud_parse_expr(lex, alloc));

        lex_next(lex);

        ud_expect(lex, UDLEX_SEMICOLON);
    }

    else if (lex_expect(lex, UDLEX_LCBRACE))
    {
        lex_next(lex);

        while (lex_curr(lex).type != UDLEX_RCBRACE)
        {
            UdAst ast = ud_parse(lex, alloc);

            ret.func.body.data = ast.data;
            ret.func.body.len = ast.len;

            lex_next(lex);
        }
    }

    return ret;
}

UdAstNode ud_parse_decl(Lex *lex, Alloc *alloc)
{
    UdAstNode ret = {};

    ret.type = UD_NODE_STMT;
    ret.stmt.type = UD_STMT_DECL;
    ret.stmt.decl_.type = UD_DECL_NONE;

    if (lex_expect(lex, UDLEX_LET))
    {
        ret.stmt.decl_ = ud_parse_var_decl(lex, alloc);
    }

    else if (lex_expect(lex, UDLEX_FUNC))
    {
        ret.stmt.decl_ = ud_parse_func_decl(lex, alloc);
    }

    return ret;
}
