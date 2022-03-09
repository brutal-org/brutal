#include <cc/builder/expr.h>
#include <brutal-debug>

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

static const char *cexpr_to_str_raw[CEXPR_COUNT] = {
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

Str cexpr_to_str(CExprType type)
{
    return str$(cexpr_to_str_raw[type]);
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

/* --- Builder -------------------------------------------------------------- */

CExpr cexpr_empty(void)
{
    return (CExpr){
        .type = CEXPR_EMPTY,
    };
}

CExpr cexpr_constant(CVal val)
{
    return (CExpr){
        .type = CEXPR_CONSTANT,
        .constant_ = val,
    };
}

CExpr cexpr_self(void)
{
    return (CExpr){
        .type = CEXPR_SELF,
    };
}

CExpr cexpr_ident(Str ident)
{
    return (CExpr){
        .type = CEXPR_IDENT,
        .ident_ = ident,
    };
}

CExpr cexpr_call(Alloc *alloc, CExpr name)
{
    CExpr call;
    call.type = CEXPR_CALL;
    call.call_.expr = alloc_move(alloc, name);

    vec_init(&call.call_.args, alloc);

    return call;
}

CExpr cexpr_infix(CExpr left, COp type, CExpr right, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_INFIX,
        .infix_ = {
            .op = type,
            .lhs = alloc_move(alloc, left),
            .rhs = alloc_move(alloc, right),
        },
    };
}

CExpr cexpr_prefix(COp type, CExpr left, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_PREFIX,
        .prefix_ = {
            .op = type,
            .expr = alloc_move(alloc, left),
        },
    };
}

CExpr cexpr_postfix(CExpr left, COp type, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_POSTFIX,
        .postfix_ = {
            .op = type,
            .expr = alloc_move(alloc, left),
        },
    };
}

CExpr cexpr_cast(CExpr expr, CType type, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_CAST,
        .cast_ =
            {
                .type = type,
                .expr = alloc_move(alloc, expr),
            },
    };
}

CExpr cexpr_ternary(CExpr cond, CExpr etrue, CExpr efalse, Alloc *alloc)
{
    return (CExpr){
        .type = CEXPR_TERNARY,
        .ternary_ =
            {
                .expr_cond = alloc_move(alloc, cond),
                .expr_true = alloc_move(alloc, etrue),
                .expr_false = alloc_move(alloc, efalse),
            },
    };
}

CExpr cexpr_initializer(CType type, Alloc *alloc)
{
    CExpr result = {
        .type = CEXPR_INITIALIZER,
        .initializer_.type = type,
    };

    vec_init(&result.initializer_.initializer, alloc);

    return result;
}

CExpr cexpr_lambda(CType type, CStmt body, Alloc *alloc)
{
    CExpr result = {
        .type = CEXPR_LAMBDA,
        .lambda_ = {
            .type = type,
            .body = alloc_move(alloc, body),
        },
    };

    return result;
}

// ++a
CExpr cexpr_preinc(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_INC, a, alloc);
}

// --a
CExpr cexpr_predec(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_DEC, a, alloc);
}

// a++
CExpr cexpr_postinc(CExpr a, Alloc *alloc)
{
    return cexpr_postfix(a, COP_INC, alloc);
}

// a--
CExpr cexpr_postdec(CExpr a, Alloc *alloc)
{
    return cexpr_postfix(a, COP_DEC, alloc);
}

// a = b
CExpr cexpr_assign(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN, b, alloc);
}

// a += b
CExpr cexpr_assign_add(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_ADD, b, alloc);
}

// a -= b
CExpr cexpr_assign_sub(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_SUB, b, alloc);
}

// a *= b
CExpr cexpr_assign_mul(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_MULT, b, alloc);
}

// a /= b
CExpr cexpr_assign_div(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_DIV, b, alloc);
}

// a %= b
CExpr cexpr_assign_mod(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_MOD, b, alloc);
}

// a &= b
CExpr cexpr_assign_and(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_BIT_AND, b, alloc);
}

// a |= b
CExpr cexpr_assign_or(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_BIT_OR, b, alloc);
}

// a ^= b
CExpr cexpr_assign_xor(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_BIT_XOR, b, alloc);
}

// a <<= b
CExpr cexpr_assign_lshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_LSHIFT, b, alloc);
}

// a >>= b
CExpr cexpr_assign_rshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_RSHIFT, b, alloc);
}

// a + b
CExpr cexpr_add(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ADD, b, alloc);
}

// a - b
CExpr cexpr_sub(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_SUB, b, alloc);
}

// a * b
CExpr cexpr_mul(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_MULT, b, alloc);
}

// a / b
CExpr cexpr_div(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_DIV, b, alloc);
}

// a % b
CExpr cexpr_mod(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_MOD, b, alloc);
}

// ~(a)
CExpr cexpr_bit_not(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_BIT_NOT, a, alloc);
}

// a & b
CExpr cexpr_bit_and(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_BIT_AND, b, alloc);
}

// a | b
CExpr cexpr_bit_or(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_BIT_OR, b, alloc);
}

// a ^ b
CExpr cexpr_bit_xor(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_BIT_XOR, b, alloc);
}

// a << b
CExpr cexpr_lshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_LSHIFT, b, alloc);
}

// a >> b
CExpr cexpr_rshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_RSHIFT, b, alloc);
}

// !(a)
CExpr cexpr_not(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_NOT, a, alloc);
}

// a && b
CExpr cexpr_and(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_AND, b, alloc);
}

// a || b
CExpr cexpr_or(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_OR, b, alloc);
}

// a == b
CExpr cexpr_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_EQ, b, alloc);
}

// a != b
CExpr cexpr_not_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_NOT_EQ, b, alloc);
}

// a < b
CExpr cexpr_lower(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_LT, b, alloc);
}

// a > b
CExpr cexpr_greater(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_GT, b, alloc);
}

// a >= b
CExpr cexpr_greater_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_GT_EQ, b, alloc);
}

// a <= b
CExpr cexpr_lower_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_LT_EQ, b, alloc);
}

// a[b]
CExpr cexpr_index(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_INDEX, b, alloc);
}

// *a
CExpr cexpr_deref(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_DEREF, a, alloc);
}

// &a
CExpr cexpr_ref(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_REF, a, alloc);
}

// a.b
CExpr cexpr_access(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ACCESS, b, alloc);
}

// a->b
CExpr cexpr_ptr_access(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_PTR_ACCESS, b, alloc);
}

void cexpr_member(CExpr *self, CExpr expr)
{
    if (self->type == CEXPR_CALL)
    {
        vec_push(&self->call_.args, expr);
    }
    else if (self->type == CEXPR_INITIALIZER)
    {
        vec_push(&self->initializer_.initializer, expr);
    }
    else
    {
        panic$("Invalid use of cexpr_member");
    }
}
