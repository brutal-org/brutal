#pragma once

#include <cc/ast/expr.h>
#include <brutal-base>
#include <brutal-ds>
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
    CSTMT_CONTINUE,

    CSTMT_LABEL,
    CSTMT_CASE,
    CSTMT_DEFAULT,
    CSTMT_COUNT
} CStmtType;

typedef struct _CStmt CStmt;

struct _CStmt
{
    CStmtType type;
    union
    {
        struct
        {
            struct _CDecl *decl;
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
            CStmt *init_stmt;
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

Str cstmt_type_to_str(CStmtType type);
