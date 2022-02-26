#pragma once

#include <brutal/text.h>
#include <cc/ast.h>
#include <cc/sema.h>

typedef enum
{
    CSEMA_CHECK_NONE,
    CSEMA_CHECK_EXPR,
    CSEMA_CHECK_DECL,
    CSEMA_CHECK_STMT,
    CSEMA_CHECK_TYPE,
    CSEMA_CHECK_UNIT,
} CSemaCheckTypes;

typedef void (*CSemaCheckExpr)(CSema *self, CExpr *, void *ctx);
typedef void (*CSemaCheckDecl)(CSema *self, CDecl *, void *ctx);
typedef void (*CSemaCheckStmt)(CSema *self, CStmt *, void *ctx);
typedef void (*CSemaCheckType)(CSema *self, CType *, void *ctx);
typedef void (*CSemaCheckUnit)(CSema *self, CUnit *, void *ctx);

typedef struct
{
    const char *name;
    CSemaCheckTypes type;
    void *ctx;

    union
    {
        CSemaCheckExpr _expr;
        struct
        {

            CSemaCheckDecl fn;
            // if this is true, then push the declaration in the context before calling fn
            // if this is false, it will push the declaration in the context after calling fn
            bool include_recursive_entry;
        } _decl;
        CSemaCheckStmt _stmt;
        CSemaCheckType _type;
        CSemaCheckUnit _unit;
    };
} SemaCheck;

void do_sema_check(SemaCheck *check, CUnit *unit, CSema *sema);

void do_sema_checks(CUnit *unit, CSema *sema);
