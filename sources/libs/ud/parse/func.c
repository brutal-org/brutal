#include <brutal/debug.h>
#include <ud/parse/decl.h>
#include <ud/parse/lexer.h>
#include "brutal/parse/lex.h"

UdFuncCall ud_parse_func_call(Lex *lex, Alloc *alloc)
{
    UdFuncCall ret = {};

    vec_init(&ret.params, alloc);

    ret.name = lex_curr(lex).str;

    lex_next(lex);

    ud_parse_whitespace(lex);

    lex_next(lex);

    while (lex_curr(lex).type != UDLEX_RPAREN)
    {
        ud_parse_whitespace(lex);

        vec_push(&ret.params, ud_parse_expr(lex, alloc).expr);

        lex_next(lex);

        ud_parse_whitespace(lex);

        if (lex_peek(lex, 1).type != UDLEX_RPAREN)
        {
            ud_parse_whitespace(lex);

            if (lex_curr(lex).type != UDLEX_RPAREN)
            {
                ud_parse_whitespace(lex);

                ud_expect(lex, UDLEX_COMMA);
            }

            else
            {
                break;
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
    }

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
        param.name = str_dup(lex_curr(lex).str, alloc);

        ud_expect(lex, UDLEX_IDENT);

        // :
        ud_parse_whitespace(lex);

        ud_expect(lex, UDLEX_COLON);

        ud_parse_whitespace(lex);

        // baz
        param.type.name = str_dup(lex_next(lex).str, alloc);

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
            UdAstNode node = ud_parse(lex, alloc);

            vec_push(&ret.func.body, node);

            if (lex_curr_type(lex) == UDLEX_RCBRACE)
                break;

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

        ud_expect(lex, UDLEX_RCBRACE);
    }

    return ret;
}
