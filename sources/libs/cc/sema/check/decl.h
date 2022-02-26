#pragma once

#include <cc/sema/checks.h>

void sema_decl_redef_check(CSema *self, CDecl *decl, void *ctx);

void sema_decl_ref_check_expr(CSema *self, CExpr *expr, void *ctx);

#define SEMA_DECL_DEF_VALIDATION                 \
    {                                            \
        .name = ("decl redefinition"),           \
        .type = CSEMA_CHECK_DECL,                \
        ._decl = {sema_decl_redef_check, false}, \
    }

#define SEMA_DECL_REF_VALIDATION           \
    {                                      \
        .name = ("decl reference"),        \
        .type = CSEMA_CHECK_EXPR,          \
        ._expr = sema_decl_ref_check_expr, \
    }
