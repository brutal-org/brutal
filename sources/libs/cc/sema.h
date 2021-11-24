#pragma once

#include <cc/ast/unit.h>

CVal csema_value(CVal value);

CType csema_type(CType type);

CExpr csema_expr(CExpr expr);

CStmt csema_stmt(CStmt stmt);

CDecl csema_decl(CDecl decl);

CUnit csema_unit(CUnit unit);
