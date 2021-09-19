#pragma once

#include <brutal/io.h>
#include <cc/ast/unit.h>

void c2c_value(Emit *target, CVal value);

void c2c_type(Emit *target, CType type);

void c2c_expr(Emit *target, CExpr expr);

void c2c_stmt(Emit *target, CStmt stmt);

void c2c_decl(Emit *target, CDecl decl);

void c2c_unit(Emit *target, CUnit unit);
