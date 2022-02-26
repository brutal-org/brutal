#pragma once
#include <cc/sema/checks.h>

void sema_deref_check_expr(CSema *self, CExpr *cexpr, void *ctx);

#define SEMA_DEREF_VALIDATION           \
    {                                   \
        .name = ("deref type"),         \
        .type = CSEMA_CHECK_EXPR,       \
        ._expr = sema_deref_check_expr, \
    }
