#pragma once

#include <bruwutal/io.h>
#include <cc/ast/unit.h>
#include <json/objects.h>

Json cdump_value(CVal value, Alloc *alloc);

Json cdump_type(CType type, Alloc *alloc);

Json cdump_expr(CExpr expr, Alloc *alloc);

Json cdump_stmt(CStmt stmt, Alloc *alloc);

Json cdump_decl(CDecl decl, Alloc *alloc);

Json cdump_unit(CUnit unit, Alloc *alloc);
