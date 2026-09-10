#pragma once

#include <cc/ast.h>
#include <cc/sema/sema.h>

#define CVISIT_DECL (1 << 0)
#define CVISIT_EXPR (1 << 1)
#define CVISIT_STMT (1 << 2)
#define CVISIT_TYPE (1 << 3)
#define CVISIT_UNIT (1 << 4)

#define CVISIT_BEGIN (1 << 5)
#define CVISIT_END (1 << 6)

typedef void CVisitFn(CSema *ctx, int what, void *node);

typedef struct
{
    CVisitFn *fn;
    int what;
} CVisit;

typedef Slice(CVisit) CVisites;

bool cvisit_node_is(void *node, int what, int type);

void cvisit_before(CVisites passes, CSema *ctx, int what, void *node);

void cvisit_after(CVisites passes, CSema *ctx, int what, void *node);

void cvisit_decl(CVisites passes, CSema *ctx, CDecl *decl);

void cvisit_expr(CVisites passes, CSema *ctx, CExpr *expr);

void cvisit_stmt(CVisites passes, CSema *ctx, CStmt *stmt);

void cvisit_type(CVisites passes, CSema *ctx, CType *type);

void cvisit_unit(CVisites passes, CSema *ctx, CUnit *unit);
