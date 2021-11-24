#pragma once

#include <brutal/io.h>
#include <cc/ast/unit.h>

void cdump_value(Emit *target, CVal value);

void cdump_type(Emit *target, CType type);

void cdump_expr(Emit *target, CExpr expr);

void cdump_stmt(Emit *target, CStmt stmt);

void cdump_decl(Emit *target, CDecl decl);

void cdump_unit(Emit *target, CUnit unit);
