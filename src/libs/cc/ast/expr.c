#include <cc/ast/expr.h>

static const char *cop_type_to_str[COP_COUNT] = {
    [COP_INVALID] = "invalid",
    [COP_INC] = "++",
    [COP_DEC] = "--",
    [COP_ASSIGN] = "=",
    [COP_ASSIGN_ADD] = "+=",
    [COP_ASSIGN_SUB] = "-=",
    [COP_ASSIGN_MULT] = "*=",
    [COP_ASSIGN_DIV] = "/=",
    [COP_ASSIGN_MOD] = "%=",
    [COP_ASSIGN_BIT_AND] = "&=",
    [COP_ASSIGN_BIT_OR] = "|=",
    [COP_ASSIGN_BIT_XOR] = "^=",
    [COP_ASSIGN_LSHIFT] = "<<=",
    [COP_ASSIGN_RSHIFT] = ">>=",
    [COP_ADD] = "+",
    [COP_SUB] = "-",
    [COP_MULT] = "*",
    [COP_DIV] = "/",
    [COP_MOD] = "%",
    [COP_BIT_NOT] = "~",
    [COP_BIT_AND] = "&",
    [COP_BIT_OR] = "|",
    [COP_BIT_XOR] = "^",
    [COP_LSHIFT] = "<<",
    [COP_RSHIFT] = ">>",
    [COP_NOT] = "!",
    [COP_AND] = "&&",
    [COP_OR] = "||",
    [COP_EQ] = "==",
    [COP_NOT_EQ] = "!=",
    [COP_LT] = "<",
    [COP_GT] = ">",
    [COP_LT_EQ] = "<=",
    [COP_GT_EQ] = ">=",
    [COP_INDEX] = "[",
    [COP_DEREF] = "*",
    [COP_REF] = "&",
    [COP_ACCESS] = ".",
    [COP_PTR_ACCESS] = "->",
};

Str cop_to_str(COp type)
{
    return str$(cop_type_to_str[type]);
}

static const char *cexpr_type_to_str_raw[CEXPR_COUNT] = {
    [CEXPR_EMPTY] = "empty",
    [CEXPR_SELF] = "self",
    [CEXPR_CONSTANT] = "constant",
    [CEXPR_IDENT] = "identifier",
    [CEXPR_PREFIX] = "prefix",
    [CEXPR_INFIX] = "infix",
    [CEXPR_POSTFIX] = "postfix",
    [CEXPR_CALL] = "call",
    [CEXPR_CAST] = "cast",
    [CEXPR_TERNARY] = "ternary",
    [CEXPR_INITIALIZER] = "initializer",
    [CEXPR_LAMBDA] = "lambda",
};

Str cexpr_type_to_str(CExprType type)
{
    return str$(cexpr_type_to_str_raw[type]);
}

static int cop_precedence_table[COP_COUNT] = {
    [COP_INC] = 1,
    [COP_DEC] = 1,
    [COP_ASSIGN] = 14,
    [COP_ASSIGN_ADD] = 14,
    [COP_ASSIGN_SUB] = 14,
    [COP_ASSIGN_MULT] = 14,
    [COP_ASSIGN_DIV] = 14,
    [COP_ASSIGN_MOD] = 14,
    [COP_ASSIGN_BIT_AND] = 14,
    [COP_ASSIGN_BIT_OR] = 14,
    [COP_ASSIGN_BIT_XOR] = 14,
    [COP_ASSIGN_LSHIFT] = 14,
    [COP_ASSIGN_RSHIFT] = 14,
    [COP_ADD] = 2,
    [COP_SUB] = 2,
    [COP_MULT] = 3,
    [COP_DIV] = 3,
    [COP_MOD] = 3,
    [COP_BIT_NOT] = 2,
    [COP_BIT_AND] = 8,
    [COP_BIT_OR] = 10,
    [COP_BIT_XOR] = 9,
    [COP_LSHIFT] = 5,
    [COP_RSHIFT] = 5,
    [COP_NOT] = 2,
    [COP_AND] = 12,
    [COP_OR] = 11,
    [COP_EQ] = 7,
    [COP_NOT_EQ] = 7,
    [COP_LT] = 6,
    [COP_GT] = 6,
    [COP_LT_EQ] = 6,
    [COP_GT_EQ] = 6,
    [COP_INDEX] = 1,
    [COP_DEREF] = 2,
    [COP_REF] = 2,
    [COP_ACCESS] = 1,
    [COP_PTR_ACCESS] = 1,
};

int cop_pre(COp cop)
{
    return cop_precedence_table[cop];
}

int cexpr_pre(CExpr *expr)
{
    switch (expr->type)
    {
    case CEXPR_PREFIX:
    case CEXPR_POSTFIX:
    case CEXPR_INFIX:
        return cop_pre(expr->infix_.op);

    case CEXPR_IDENT:
    case CEXPR_CONSTANT:
        return 0;

    case CEXPR_CALL:
    case CEXPR_CAST:
    case CEXPR_INITIALIZER:
        return 1;

    case CEXPR_TERNARY:
        return 13;

    default:
        return 0;
    }
}
