#pragma once

#include <bs/bs.h>

/* --- Syntax ------------------------------------------- */

BsExpr bs_builtin_define(BsExpr args, BsExpr *env, Alloc *alloc);

BsExpr bs_builtin_lambda(BsExpr args, BsExpr *env, Alloc *alloc);

BsExpr bs_builtin_block(BsExpr args, BsExpr *env, Alloc *alloc);
