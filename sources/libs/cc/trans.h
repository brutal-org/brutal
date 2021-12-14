#pragma once

#include <brutal/io.h>
#include <cc/ast/unit.h>

void ctran_decl(Emit *emit, CDecl decl);

void ctran_expr(Emit *emit, CExpr expr);

void ctran_stmt(Emit *emit, CStmt stmt);

void ctran_type(Emit *emit, CType type);

void ctran_type_start(Emit *emit, CType type);

void ctran_func_params(Emit *emit, CType type);

void ctran_type_end(Emit *emit, CType type);

void ctran_unit(Emit *emit, CUnit unit);

void ctran_value(Emit *emit, CVal value);
