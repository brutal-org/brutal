#pragma once

#include <cc/ast/unit.h>
#include <cc/sema/sema.h>

CVal csema_value(CSema *sema, CVal value);

CType csema_type(CSema *sema, CType type);

CExpr csema_expr(CSema *sema, CExpr expr);

CStmt csema_stmt(CSema *sema, CStmt stmt);

CDecl csema_decl(CSema *sema, CDecl decl);

CUnit csema_unit(CSema *sema, CUnit unit);