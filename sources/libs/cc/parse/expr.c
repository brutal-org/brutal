#include <cc/builder.h>
#include <cc/parse/parser.h>
#include <brutal-debug>
#include <brutal-parse>

static COp lexeme_to_cop[CLEX_COUNT] = {
    [CLEX_PLUSPLUS] = COP_INC,
    [CLEX_MINUSMINUS] = COP_DEC,
    [CLEX_EQUAL] = COP_ASSIGN,
    [CLEX_PLUS_EQ] = COP_ASSIGN_ADD,
    [CLEX_MINUS_EQ] = COP_ASSIGN_SUB,
    [CLEX_STAR_EQ] = COP_ASSIGN_MULT,
    [CLEX_SLASH_EQ] = COP_ASSIGN_DIV,
    [CLEX_PERCENT_EQ] = COP_ASSIGN_MOD,
    [CLEX_AMPERSAND_EQ] = COP_ASSIGN_BIT_AND,
    [CLEX_BAR_EQ] = COP_ASSIGN_BIT_OR,
    [CLEX_CIRCUMFLEX_EQ] = COP_ASSIGN_BIT_XOR,
    [CLEX_LSHIFT_EQ] = COP_ASSIGN_LSHIFT,
    [CLEX_RSHIFT_EQ] = COP_ASSIGN_RSHIFT,
    [CLEX_PLUS] = COP_ADD,
    [CLEX_MINUS] = COP_SUB,
    [CLEX_STAR] = COP_MULT,
    [CLEX_SLASH] = COP_DIV,
    [CLEX_PERCENT] = COP_MOD,
    [CLEX_TILDE] = COP_BIT_NOT,
    [CLEX_AMPERSAND] = COP_BIT_AND,
    [CLEX_BAR] = COP_BIT_OR,
    [CLEX_CIRCUMFLEX] = COP_BIT_XOR,
    [CLEX_LSHIFT] = COP_LSHIFT,
    [CLEX_RSHIFT] = COP_RSHIFT,
    [CLEX_EXCLAMATION] = COP_NOT,
    [CLEX_AMPERSANDAMPERSAND] = COP_AND,
    [CLEX_BARBAR] = COP_OR,
    [CLEX_EQUALEQUAL] = COP_EQ,
    [CLEX_EXCLAMATIONEQUAL] = COP_NOT_EQ,
    [CLEX_LCHEVRON] = COP_LT,
    [CLEX_RCHEVRON] = COP_GT,
    [CLEX_LCHEVRON_EQ] = COP_LT_EQ,
    [CLEX_RCHEVRON_EQ] = COP_GT_EQ,
    [CLEX_LBRACKET] = COP_INDEX,
    [CLEX_DOT] = COP_ACCESS,
    [CLEX_ARROW] = COP_PTR_ACCESS,
};

COp cparse_lexeme_to_cop(LexemeType type)
{
    return lexeme_to_cop[type];
}

static CExpr cparse_parent_expr(Lex *lex, CUnit *context, Alloc *alloc)
{
    CExpr result;

    cparse_whitespace(lex);
    if (lex_curr_type(lex) == CLEX_IDENT && cunit_contains_type(context, lex_curr(lex).str))
    {
        CType cast_type = cparse_declarator(lex, alloc).type;
        cparse_skip_separator(lex, CLEX_RPARENT);
        result = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
        result = cexpr_cast(result, cast_type, alloc);
    }
    else
    {
        result = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
        cparse_skip_separator(lex, CLEX_RPARENT);
    }

    return result;
}
static CExpr cparse_primary_expr(Lex *lex, CUnit *context, Alloc *alloc)
{
    if (cparse_is_separator(lex, CLEX_IDENT))
    {
        Str val = lex_curr(lex).str;
        cparse_skip_separator(lex, CLEX_IDENT);
        return cexpr_ident(val);
    }

    else if (cparse_is_separator(lex, CLEX_INTEGER))
    {
        return cexpr_constant(cparse_val(lex));
    }

    else if (cparse_skip_separator(lex, CLEX_LPARENT))
    {
        CExpr expr = cparse_parent_expr(lex, context, alloc);
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

        CStmt body = cparse_stmt(lex, context, alloc);

        return cexpr_lambda(type, body, alloc);
    }
    else
    {
        lex_throw(lex, str$("Unexpect token!"));
        return cexpr_empty();
    }
    return cexpr_constant(cval$(0));
}

static void cparse_func_call_args(Lex *lex, CExpr *target, CUnit *context, Alloc *alloc)
{

    if (!cparse_skip_separator(lex, CLEX_RPARENT))
    {
        do
        {
            CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
            cexpr_member(target, expr);
        } while (cparse_skip_separator(lex, CLEX_COMMA));

        cparse_expect_separator(lex, CLEX_RPARENT);
    }
}
static void cparse_postfix_expr(Lex *lex, CExpr *target, CUnit *context, Alloc *alloc)
{

    if (cparse_skip_separator(lex, CLEX_LPARENT))
    {
        *target = cexpr_call(alloc, *target);
        cparse_func_call_args(lex, target, context, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_LBRACKET))
    {
        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
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
static CExpr cparse_prefix_expr(Lex *lex, CUnit *context, Alloc *alloc)
{
    CExpr result;

    // FIXME: put every parse prefix
    if (cparse_skip_separator(lex, CLEX_MINUS))
    {

        result = cparse_prefix_expr(lex, context, alloc);
        result = cexpr_sub(cexpr_empty(), result, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_STAR))
    {
        result = cparse_prefix_expr(lex, context, alloc);
        result = cexpr_deref(result, alloc);
    }
    else
    {
        result = cparse_primary_expr(lex, context, alloc);
        cparse_postfix_expr(lex, &result, context, alloc);
    }

    return result;
}

bool cparse_is_end_expr(LexemeType type)
{
    return type == CLEX_RPARENT || type == CLEX_SEMICOLON || type == CLEX_COMMA || type == CLEX_RBRACKET;
}

CExpr cparse_expr(Lex *lex, int pre, CUnit *context, Alloc *alloc)
{
    CExpr left = cparse_prefix_expr(lex, context, alloc);
    CExpr right;

    cparse_whitespace(lex);
    Lexeme curr_lex = lex_curr(lex);

    if (cparse_is_end_expr(curr_lex.type))
    {
        return left;
    }

    while (!lex_ended(lex) && cop_pre(cparse_lexeme_to_cop(curr_lex.type)) < pre)
    {
        COp curr_cop = cparse_lexeme_to_cop(curr_lex.type);

        lex_next(lex);
        cparse_whitespace(lex);

        right = cparse_expr(lex, cop_pre(curr_cop), context, alloc);

        left = cexpr_infix(left, curr_cop, right, alloc);

        curr_lex = lex_curr(lex);

        if (cparse_is_end_expr(curr_lex.type))
        {
            return left;
        }
    }
    return left;
}

/*
}
*/
