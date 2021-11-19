#pragma once

#include <cc/ast/decl.h>
#include <cc/ast/stmt.h>

CStmt cstmt_empty(void);

CStmt cstmt_decl(CDecl decl, Alloc *alloc);

CStmt cstmt_expr(CExpr expr);

CStmt cstmt_block(Alloc *alloc);

void cstmt_block_add(CStmt *self, CStmt statement);

CStmt cstmt_if(CExpr expr, CStmt stmt_true, Alloc *alloc);

CStmt cstmt_if_else(CExpr expr, CStmt stmt_true, CStmt stmt_false, Alloc *alloc);

CStmt cstmt_for(CStmt init_stmt, CExpr cond_expr, CExpr iter_expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_while(CExpr expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_do(CExpr expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_switch(CExpr expr, CStmt stmt, Alloc *alloc);

CStmt cstmt_return(CExpr expr);

CStmt cstmt_goto(Str label);

CStmt cstmt_label(Str label);

CStmt cstmt_case(CExpr expr);

CStmt cstmt_default(void);

CStmt cstmt_break(void);
