#include <brutal/debug.h>
#include <brutal/parse/nums.h>
#include <cc/builder.h>
#include <cc/parse/parser.h>

static CExpr cparse_parent_expr(Lex *lex, Alloc *alloc)
{

    CExpr result;

    result = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

    return result;
}
static CExpr cparse_primary_expr(Lex *lex, Alloc *alloc)
{
    if (cparse_is_separator(lex, CLEX_IDENT))
    {
        Str val = lex_curr(lex).str;
        cparse_skip_separator(lex, CLEX_IDENT);
        return cexpr_ident(val);
    }

    else if (cparse_is_separator(lex, CLEX_INTEGER))
    {
        long val = str_to_number(lex_curr(lex).str);
        cparse_skip_separator(lex, CLEX_INTEGER);
        return cexpr_constant(cval$(val));
    }

    else if (cparse_skip_separator(lex, CLEX_LPARENT))
    {
        CExpr expr = cparse_parent_expr(lex, alloc);
        cparse_skip_separator(lex, CLEX_RPARENT);
        return expr;
    }

    else if (cparse_skip_separator(lex, CLEX_LBRACKET))
    {
        // TODO: parse lambda capture
        cparse_skip_separator(lex, CLEX_RBRACKET);

        CType type = ctype_func(ctype_auto(), alloc);

        cparse_skip_separator(lex, CLEX_LPARENT);
        cparse_func_params(lex, &type, alloc);
        cparse_skip_separator(lex, CLEX_RPARENT);

        CStmt body = cparse_stmt(lex, alloc);

        return cexpr_lambda(type, body, alloc);
    }
    else
    {
        lex_throw(lex, str$("Unexpect token!"));
        return cexpr_empty();
    }
    return cexpr_constant(cval$(0));
}

static void cparse_func_call_args(Lex *lex, CExpr *target, Alloc *alloc)
{

    if (!cparse_skip_separator(lex, CLEX_RPARENT))
    {
        do
        {
            CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);
            cexpr_member(target, expr);
        } while (cparse_skip_separator(lex, CLEX_COMMA));

        cparse_expect_separator(lex, CLEX_RPARENT);
    }
}
static void cparse_postfix_expr(Lex *lex, CExpr *target, Alloc *alloc)
{

    if (cparse_skip_separator(lex, CLEX_LPARENT))
    {
        *target = cexpr_call(alloc, *target);
        cparse_func_call_args(lex, target, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_LBRACKET))
    {
        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);
        cparse_skip_separator(lex, CLEX_RBRACKET);
        *target = cexpr_index(*target, expr, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_PLUSPLUS))
    {
        *target = cexpr_postinc(*target, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_MINUSMINUS))
    {
        *target = cexpr_postdec(*target, alloc);
    }
}
static CExpr cparse_prefix_expr(Lex *lex, Alloc *alloc)
{
    CExpr result;

    // FIXME: put every parse prefix
    if (cparse_skip_separator(lex, CLEX_MINUS))
    {

        result = cparse_prefix_expr(lex, alloc);
        result = cexpr_sub(cexpr_empty(), result, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_STAR))
    {
        result = cparse_prefix_expr(lex, alloc);
        result = cexpr_deref(result, alloc);
    }
    else
    {
        result = cparse_primary_expr(lex, alloc);
        cparse_postfix_expr(lex, &result, alloc);
    }

    return result;
}

CExpr cparse_expr(Lex *lex, int pre, Alloc *alloc)
{
    CExpr left = cparse_prefix_expr(lex, alloc);
    CExpr right;

    cparse_whitespace(lex);
    Lexeme curr_lex = lex_curr(lex);

    if (curr_lex.type == CLEX_RPARENT || curr_lex.type == CLEX_SEMICOLON || curr_lex.type == CLEX_COMMA || curr_lex.type == CLEX_RBRACKET)
    {
        return left;
    }

    while (cop_pre(cparse_lexeme_to_cop(curr_lex.type)) < pre)
    {
        COp curr_cop = cparse_lexeme_to_cop(curr_lex.type);

        lex_next(lex);
        cparse_whitespace(lex);

        right = cparse_expr(lex, cop_pre(curr_cop), alloc);

        left = cexpr_infix(left, curr_cop, right, alloc);

        curr_lex = lex_curr(lex);

        if (curr_lex.type == CLEX_RPARENT || curr_lex.type == CLEX_SEMICOLON || curr_lex.type == CLEX_COMMA || curr_lex.type == CLEX_RBRACKET)
        {
            return left;
        }
    }
    return left;
}

/*
}
*/
