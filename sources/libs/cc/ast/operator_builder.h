#pragma once
#include <cc/ast/builder.h>

// ++a
static inline CExpr cexpr_inc_l(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_INC, a, alloc);
}

// --a
static inline CExpr cexpr_dec_l(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_DEC, a, alloc);
}

// a++
static inline CExpr cexpr_inc_r(CExpr a, Alloc *alloc)
{
    return cexpr_postfix(a, COP_INC, alloc);
}

// a--
static inline CExpr cexpr_dec_r(CExpr a, Alloc *alloc)
{
    return cexpr_postfix(a, COP_DEC, alloc);
}

// a = b
static inline CExpr cexpr_assign(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN, b, alloc);
}

// a += b
static inline CExpr cexpr_assign_add(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_ADD, b, alloc);
}

// a -= b
static inline CExpr cexpr_assign_sub(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_SUB, b, alloc);
}

// a *= b
static inline CExpr cexpr_assign_mul(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_MULT, b, alloc);
}

// a /= b
static inline CExpr cexpr_assign_div(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_DIV, b, alloc);
}

// a %= b
static inline CExpr cexpr_assign_mod(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_MOD, b, alloc);
}

// a &= b
static inline CExpr cexpr_assign_and(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_BIT_AND, b, alloc);
}

// a |= b
static inline CExpr cexpr_assign_or(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_BIT_OR, b, alloc);
}

// a ^= b
static inline CExpr cexpr_assign_xor(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_BIT_XOR, b, alloc);
}

// a <<= b
static inline CExpr cexpr_assign_lshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_LSHIFT, b, alloc);
}

// a >>= b
static inline CExpr cexpr_assign_rshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ASSIGN_RSHIFT, b, alloc);
}

// a + b
static inline CExpr cexpr_add(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ADD, b, alloc);
}

// a - b
static inline CExpr cexpr_sub(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_SUB, b, alloc);
}

// a * b
static inline CExpr cexpr_mul(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_MULT, b, alloc);
}

// a / b
static inline CExpr cexpr_div(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_DIV, b, alloc);
}

// a % b
static inline CExpr cexpr_mod(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_MOD, b, alloc);
}

// ~(a)
static inline CExpr cexpr_bit_not(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_BIT_NOT, a, alloc);
}

// a & b
static inline CExpr cexpr_bit_and(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_BIT_AND, b, alloc);
}

// a | b
static inline CExpr cexpr_bit_or(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_BIT_OR, b, alloc);
}

// a ^ b
static inline CExpr cexpr_bit_xor(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_BIT_XOR, b, alloc);
}

// a << b
static inline CExpr cexpr_lshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_LSHIFT, b, alloc);
}

// a >> b
static inline CExpr cexpr_rshift(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_RSHIFT, b, alloc);
}

// !(a)
static inline CExpr cexpr_not(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_NOT, a, alloc);
}

// a && b
static inline CExpr cexpr_and(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_AND, b, alloc);
}

// a || b
static inline CExpr cexpr_or(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_OR, b, alloc);
}

// a == b
static inline CExpr cexpr_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_EQ, b, alloc);
}

// a != b
static inline CExpr cexpr_not_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_NOT_EQ, b, alloc);
}

// a < b
static inline CExpr cexpr_lower(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_LT, b, alloc);
}

// a > b
static inline CExpr cexpr_greater(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_GT, b, alloc);
}

// a >= b
static inline CExpr cexpr_greater_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_GT_EQ, b, alloc);
}

// a <= b
static inline CExpr cexpr_lower_eq(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_LT_EQ, b, alloc);
}

// a[b]
static inline CExpr cexpr_index(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_INDEX, b, alloc);
}

// *a
static inline CExpr cexpr_deref(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_DEREF, a, alloc);
}

// &a
static inline CExpr cexpr_ref(CExpr a, Alloc *alloc)
{
    return cexpr_prefix(COP_REF, a, alloc);
}

// a.b
static inline CExpr cexpr_access(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_ACCESS, b, alloc);
}

// a->b
static inline CExpr cexpr_ptr_access(CExpr a, CExpr b, Alloc *alloc)
{
    return cexpr_infix(a, COP_PTR_ACCESS, b, alloc);
}
