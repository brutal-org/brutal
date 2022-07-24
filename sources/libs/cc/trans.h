#pragma once

#include <cc/ast/unit.h>
#include <brutal-io>

void ctrans_decl(Emit *emit, CDecl decl);

void ctrans_expr(Emit *emit, CExpr expr);

void ctrans_stmt(Emit *emit, CStmt stmt);

void ctrans_type(Emit *emit, CType type);

void ctrans_type_start(Emit *emit, CType type);

void ctrans_func_params(Emit *emit, CType type);

void ctrans_type_end(Emit *emit, CType type);

Str ctrans_type_str(CExpr expr, Alloc *alloc);

void ctrans_unit(Emit *emit, CUnit unit);

void ctrans_value(Emit *emit, CVal value);
