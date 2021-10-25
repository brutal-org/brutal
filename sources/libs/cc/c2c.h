#pragma once

#include <brutal/io.h>
#include <cc/ast/unit.h>

void c2c_value(Emit *emit, CVal value);

void c2c_type(Emit *emit, CType type);

void c2c_expr(Emit *emit, CExpr expr);

void c2c_stmt(Emit *emit, CStmt stmt);

void c2c_decl(Emit *emit, CDecl decl);

void c2c_unit(Emit *emit, CUnit unit);
