#pragma once

#include <ud/ast/sexpr.h>

UdExpr ud_lhs(UdExpr expr);

UdExpr ud_rhs(UdExpr expr);

bool ud_eq(UdExpr lhs, UdExpr rhs);

bool ud_is(UdExpr expr, UdType type);

bool ud_lhs_is(UdExpr expr, UdType type);

bool ud_rhs_is(UdExpr expr, UdType type);

bool ud_is_truth(UdExpr expr);

bool ud_is_atom(UdExpr expr, Str atom);

bool ud_is_self_quoting(UdType type);

Str ud_type_to_string(UdType type);

#define ud_llhs(obj) ud_lhs(ud_lhs(obj))
#define ud_lrhs(obj) ud_lhs(ud_rhs(obj))
#define ud_rlhs(obj) ud_rhs(ud_lhs(obj))
#define ud_rrhs(obj) ud_rhs(ud_rhs(obj))

#define ud_lllhs(obj) ud_lhs(ud_lhs(ud_lhs(obj)))
#define ud_llrhs(obj) ud_lhs(ud_lhs(ud_rhs(obj)))
#define ud_lrlhs(obj) ud_lhs(ud_rhs(ud_lhs(obj)))
#define ud_lrrhs(obj) ud_lhs(ud_rhs(ud_rhs(obj)))

#define ud_rllhs(obj) ud_rhs(ud_lhs(ud_lhs(obj)))
#define ud_rlrhs(obj) ud_rhs(ud_lhs(ud_rhs(obj)))
#define ud_rrlhs(obj) ud_rhs(ud_rhs(ud_lhs(obj)))
#define ud_rrrhs(obj) ud_rhs(ud_rhs(ud_rhs(obj)))

#define ud_llllhs(obj) ud_lhs(ud_lhs(ud_lhs(ud_lhs(obj))))
#define ud_lllrhs(obj) ud_lhs(ud_lhs(ud_lhs(ud_rhs(obj))))
#define ud_llrlhs(obj) ud_lhs(ud_lhs(ud_rhs(ud_lhs(obj))))
#define ud_llrrhs(obj) ud_lhs(ud_lhs(ud_rhs(ud_rhs(obj))))
#define ud_lrllhs(obj) ud_lhs(ud_rhs(ud_lhs(ud_lhs(obj))))
#define ud_lrlrhs(obj) ud_lhs(ud_rhs(ud_lhs(ud_rhs(obj))))
#define ud_lrrlhs(obj) ud_lhs(ud_rhs(ud_rhs(ud_lhs(obj))))
#define ud_lrrrhs(obj) ud_lhs(ud_rhs(ud_rhs(ud_rhs(obj))))

#define ud_rlllhs(obj) ud_rhs(ud_lhs(ud_lhs(ud_lhs(obj))))
#define ud_rllrhs(obj) ud_rhs(ud_lhs(ud_lhs(ud_rhs(obj))))
#define ud_rlrlhs(obj) ud_rhs(ud_lhs(ud_rhs(ud_lhs(obj))))
#define ud_rlrrhs(obj) ud_rhs(ud_lhs(ud_rhs(ud_rhs(obj))))
#define ud_rrllhs(obj) ud_rhs(ud_rhs(ud_lhs(ud_lhs(obj))))
#define ud_rrlrhs(obj) ud_rhs(ud_rhs(ud_lhs(ud_rhs(obj))))
#define ud_rrrlhs(obj) ud_rhs(ud_rhs(ud_rhs(ud_lhs(obj))))
#define ud_rrrrhs(obj) ud_rhs(ud_rhs(ud_rhs(ud_rhs(obj))))
