#pragma once

#include <udfore/udfore.h>

UdExpr ud_builtin_define(UdExpr args, UdExpr *env, Alloc *alloc);

UdExpr ud_builtin_lambda(UdExpr args, UdExpr *env, Alloc *alloc);

UdExpr ud_builtin_block(UdExpr args, UdExpr *env, Alloc *alloc);

UdExpr ud_builtin_quote(UdExpr args, UdExpr *env, Alloc *alloc);
