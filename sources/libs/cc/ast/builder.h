#pragma once

#include <cc/ast/decl.h>
#include <cc/ast/expr.h>
#include <cc/ast/stmt.h>
#include <cc/ast/type.h>
#include <cc/ast/unit.h>
#include <cc/ast/val.h>

/* --- CDecl ---------------------------------------------------------------- */

CDecl cdecl_type(Str name, CType type, Alloc *alloc);

CDecl cdecl_var(Str name, CType type, CExpr expr, Alloc *alloc);

CDecl cdecl_func(Str name, CType type, CStmt body, Alloc *alloc);

/* --- CExpr ---------------------------------------------------------------- */

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

/* --- CStmt ---------------------------------------------------------------- */

CStmt cstmt_empty(void);

CStmt cstmt_decl(CDecl decl, Alloc *alloc);

CStmt cstmt_expr(CExpr expr);

CStmt cstmt_block(Alloc *alloc);

void cstmt_block_add(CStmt *self, CStmt statement);

CStmt cstmt_if(CExpr expr, CStmt stmt_true, Alloc *alloc);

CStmt cstmt_if_else(CExpr expr, CStmt stmt_true, CStmt stmt_false, Alloc *alloc);

CStmt cstmt_for(CStmt init_stmt, CExpr cond_expr, CExpr iter_expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_while(CExpr expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_do(CExpr expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_switch(CExpr expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_return(CExpr expr);

CStmt cstmt_goto(Str str, Alloc *alloc);

CStmt cstmt_case(CExpr expr);

/* --- CType ---------------------------------------------------------------- */

CType ctype_void(Str name, Alloc *alloc);

CType ctype_auto(Str name, Alloc *alloc);

CType ctype_bool(Str name, Alloc *alloc);

CType ctype_ptr(CType subtype, Alloc *alloc);

CType ctype_array(Str name, CType subtype, int size, Alloc *alloc);

CType ctype_signed(Str name, int precision, Alloc *alloc);

CType ctype_unsigned(Str name, int precision, Alloc *alloc);

CType ctype_float(Str name, int precision, Alloc *alloc);

CType ctype_struct(Str name, Alloc *alloc);

CType ctype_union(Str name, Alloc *alloc);

void ctype_member(CType *self, Str name, CType type, Alloc *alloc);

CType ctype_enum(Str name, Alloc *alloc);

void ctype_constant(CType *self, Str name, CVal val, Alloc *alloc);

CType ctype_func(CType ret, Str name, Alloc *alloc);

CType ctype_name(Str name, Alloc *alloc);

CType ctype_attr(CType type, CTypeAttr attr);

/* --- CVal  ---------------------------------------------------------------- */

CVal cval_signed(CSigned v);

CVal cval_unsigned(CUnsigned v);

CVal cval_float(double v);

CVal cval_string(Str str, Alloc *alloc);

/* --- CUnit ---------------------------------------------------------------- */

CUnit cunit(Alloc *alloc);

void cunit_member(CUnit *unit, CUnitEntry entry);

CUnitEntry cunit_decl(CDecl decl);

CUnitEntry cunit_pragma(Str text, Alloc *alloc);

CUnitEntry cunit_pragma_once(Alloc *alloc);

CUnitEntry cunit_include(bool qchar, Str path, Alloc *alloc);
