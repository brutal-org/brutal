#pragma once

#include <cc/sema/checks.h>

void sema_call_check_expr(CSema *self, CExpr *cexpr, void *ctx);

#define SEMA_CALL_VALIDATION           \
    {                                  \
        .name = ("call type"),         \
        .type = CSEMA_CHECK_EXPR,      \
        ._expr = sema_call_check_expr, \
    }
