#include <cc/builder/stmt.h>

CStmt cstmt_empty(void)
{
    return (CStmt){
        .type = CSTMT_EMPTY,
    };
}

CStmt cstmt_decl(CDecl decl, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_EMPTY,
        .decl_.decl = alloc_move(alloc, decl),
    };
}

CStmt cstmt_expr(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_EXPR,
        .expr_.expr = expr,
    };
}

CStmt cstmt_block(Alloc *alloc)
{
    CStmt stmt = {
        .type = CSTMT_BLOCK,
    };

    vec_init(&stmt.block_.stmts, alloc);

    return stmt;
}

void cstmt_block_add(CStmt *self, CStmt statement)
{
    if (self->type != CSTMT_BLOCK)
    {
        panic$("cstmt_block_add should only be called with block");
    }

    vec_push(&self->block_.stmts, statement);
}

CStmt cstmt_if(CExpr expr, CStmt stmt_true, Alloc *alloc)
{
    CStmt stmt_false = cstmt_empty();
    return (CStmt){
        .type = CSTMT_IF,
        .if_ = {
            .expr = expr,
            .stmt_true = alloc_move(alloc, stmt_true),
            .stmt_false = alloc_move(alloc, stmt_false),
        },
    };
}

CStmt cstmt_if_else(CExpr expr, CStmt stmt_true, CStmt stmt_false, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_IF,
        .if_ = {
            .expr = expr,
            .stmt_true = alloc_move(alloc, stmt_true),
            .stmt_false = alloc_move(alloc, stmt_false),
        },
    };
}

CStmt cstmt_for(CStmt init_stmt, CExpr cond_expr, CExpr iter_expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_FOR,
        .for_ = {
            .init_stmt = alloc_move(alloc, init_stmt),
            .cond_expr = cond_expr,
            .iter_expr = iter_expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_while(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_WHILE,
        .while_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_do(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_DO,
        .do_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_switch(CExpr expr, CStmt stmt, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_SWITCH,
        .switch_ = {
            .expr = expr,
            .stmt = alloc_move(alloc, stmt),
        },
    };
}

CStmt cstmt_return(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_RETURN,
        .return_ = {
            .expr = expr,
        },
    };
}

CStmt cstmt_goto(Str str, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_GOTO,
        .goto_.label = str_dup(str, alloc),
    };
}

CStmt cstmt_label(Str label, Alloc *alloc)
{
    return (CStmt){
        .type = CSTMT_LABEL,
        .label_.label = str_dup(label, alloc),
    };
}

CStmt cstmt_case(CExpr expr)
{
    return (CStmt){
        .type = CSTMT_CASE,
        .case_.expr = expr,
    };
}

CStmt cstmt_default(void)
{
    return (CStmt){.type = CSTMT_DEFAULT};
}
