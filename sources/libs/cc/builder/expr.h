#pragma once

#include <cc/ast/expr.h>

CExpr cexpr_empty(void);

CExpr cexpr_constant(CVal val);

CExpr cexpr_identifier(Str ident, Alloc *alloc);

CExpr cexpr_call(Alloc *alloc, CExpr name);

void cexpr_add_arg(CExpr *self, CExpr arg);

CExpr cexpr_infix(CExpr left, COp type, CExpr right, Alloc *alloc);

CExpr cexpr_prefix(COp type, CExpr left, Alloc *alloc);

CExpr cexpr_postfix(CExpr left, COp type, Alloc *alloc);

CExpr cexpr_cast(CExpr expr, CType type, Alloc *alloc);

CExpr cexpr_ternary(CExpr cond, CExpr etrue, CExpr efalse, Alloc *alloc);

CExpr cexpr_array_initializer(Alloc *alloc);

CExpr cexpr_struct_initializer(Alloc *alloc);

void cexpr_initializer_push_element(CExpr *target, CExpr designator, CExpr expr, Alloc *alloc);

CExpr cexpr_preinc(CExpr a, Alloc *alloc);

CExpr cexpr_predec(CExpr a, Alloc *alloc);

CExpr cexpr_postinc(CExpr a, Alloc *alloc);

CExpr cexpr_postdec(CExpr a, Alloc *alloc);

CExpr cexpr_assign(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_add(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_sub(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_mul(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_div(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_mod(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_and(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_or(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_xor(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_lshift(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_assign_rshift(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_add(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_sub(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_mul(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_div(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_mod(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_bit_not(CExpr a, Alloc *alloc);

CExpr cexpr_bit_and(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_bit_or(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_bit_xor(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_lshift(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_rshift(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_not(CExpr a, Alloc *alloc);

CExpr cexpr_and(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_or(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_eq(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_not_eq(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_lower(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_greater(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_greater_eq(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_lower_eq(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_index(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_deref(CExpr a, Alloc *alloc);

CExpr cexpr_ref(CExpr a, Alloc *alloc);

CExpr cexpr_access(CExpr a, CExpr b, Alloc *alloc);

CExpr cexpr_ptr_access(CExpr a, CExpr b, Alloc *alloc);
