#pragma once

#include <brutal/base.h>
#include <brutal/ds.h>
#include <cc/ast/expr.h>

typedef enum
{
    CSTMT_INVALID,
    CSTMT_EMPTY,

    CSTMT_DECL,
    CSTMT_EXPR,
    CSTMT_BLOCK,

    CSTMT_IF,
    CSTMT_FOR,
    CSTMT_WHILE,
    CSTMT_DO,
    CSTMT_SWITCH,

    CSTMT_RETURN,
    CSTMT_GOTO,
    CSTMT_BREAK,
    CSTMT_CONTINE,

    CSTMT_LABEL,
    CSTMT_CASE,
    CSTMT_DEFAULT,
} CStmtType;

typedef struct cstmt CStmt;

struct cstmt
{
    CStmtType type;

    union
    {
        struct
        {
            struct cdecl *decl;
        } decl_;

        struct
        {
            CExpr expr;
        } expr_;

        struct
        {
            Vec(CStmt) stmts;
        } block_;

        struct
        {
            CExpr expr;
            CStmt *stmt_true;
            CStmt *stmt_false;
        } if_;

        struct
        {
            CExpr init_expr;
            CExpr cond_expr;
            CExpr iter_expr;

            CStmt *stmt;
        } for_;

        struct
        {
            CExpr expr;
            CStmt *stmt;
        } while_, do_, switch_;

        struct
        {
            CExpr expr;
        } return_;

        struct
        {
            Str label;
        } goto_;

        struct
        {
            Str label;
        } label_;

        struct
        {
            CExpr expr;
        } case_;
    };
};

