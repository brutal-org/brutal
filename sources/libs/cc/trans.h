#pragma once

#include <cc/ast/unit.h>
#include <brutal-io>

void cc_trans_decl(Emit *emit, CDecl decl);

void cc_trans_expr(Emit *emit, CExpr expr);

void cc_trans_stmt(Emit *emit, CStmt stmt);

void cc_trans_type(Emit *emit, CType type);

void cc_trans_type_start(Emit *emit, CType type);

void cc_trans_func_params(Emit *emit, CType type);

void cc_trans_type_end(Emit *emit, CType type);

void cc_trans_unit(Emit *emit, CUnit unit);

void cc_trans_value(Emit *emit, CVal value);
