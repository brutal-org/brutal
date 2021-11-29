#pragma once

#include <brutal/io.h>
#include <cc/ast/unit.h>

void cgen_c_decl(Emit *emit, CDecl decl);

void cgen_c_expr(Emit *emit, CExpr expr);

void cgen_c_stmt(Emit *emit, CStmt stmt);

void cgen_c_type(Emit *emit, CType type);

void cgen_c_type_start(Emit *emit, CType type);

void cgen_c_func_params(Emit *emit, CType type);

void cgen_c_type_end(Emit *emit, CType type);

void cgen_c_unit(Emit *emit, CUnit unit);

void cgen_c_value(Emit *emit, CVal value);
