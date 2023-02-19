#pragma once

#include <cc/ast/unit.h>
#include <cc/sema/sema.h>

CVal csema_value(CSema *sema, CVal value, Alloc *alloc);

CType csema_type(CSema *sema, CType type, Alloc *alloc);

CExpr csema_expr(CSema *sema, CExpr expr, Alloc *alloc);

CStmt csema_stmt(CSema *sema, CStmt stmt, Alloc *alloc);

CDecl csema_decl(CSema *sema, CDecl decl, Alloc *alloc);

CUnit csema_unit(CSema *sema, CUnit unit, Alloc *alloc);
