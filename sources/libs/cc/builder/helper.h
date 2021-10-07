#pragma once
#include <brutal/alloc.h>
#include <cc/builder.h>

CStmt cc_var_decl_str(Str type, Str name, CExpr set, Alloc *alloc);

CExpr cc_func_call(Str call_func, Alloc *alloc);

CExpr cc_access_str(Str from, Str targ, Alloc *alloc);
CExpr cc_ptr_access_str(Str from, Str targ, Alloc *alloc);

CExpr cc_index_constant(CExpr v, int idx, Alloc *alloc);

#define cval_cast(VAL)        \
    _Generic((VAL),           \
             signed int       \
             : cval_signed,   \
               signed long    \
             : cval_signed,   \
               signed char    \
             : cval_signed,   \
               signed short   \
             : cval_signed,   \
               unsigned int   \
             : cval_unsigned, \
               unsigned long  \
             : cval_unsigned, \
               float          \
             : cval_float)(VAL)

void cc_push_initializer_member(CExpr *targ, Str designator, Str value, Alloc *alloc);

#define cexpr_str_op(type, strA, strB, ALLOC) \
    cexpr_##type(cexpr_identifier(strA, ALLOC), cexpr_identifier(strB, ALLOC), ALLOC)
