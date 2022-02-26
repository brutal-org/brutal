#pragma once

#include <cc/sema/checks.h>

void sema_array_check_expr(CSema *self, CExpr *cexpr, void *ctx);

#define SEMA_ARRAY_TYPE_VALIDATION      \
    {                                   \
        .name = ("array type"),         \
        .type = CSEMA_CHECK_EXPR,       \
        ._expr = sema_array_check_expr, \
    }
