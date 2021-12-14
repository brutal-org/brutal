#pragma once

#include <brutal/ds.h>
#include <ud/ast/val.h>

typedef enum
{
    UD_EXPR_NIL,

    UD_EXPR_VAL,
    UD_EXPR_PREFIX,
    UD_EXPR_INFIX,
    UD_EXPR_POSTFIX,

    UD_EXPR_IF,
    UD_EXPR_DO,
    UD_EXPR_WHILE,
    UD_EXPR_BLOCK,

    UD_EXPR_RET,
} UdExprType;

typedef enum
{
    UD_OP_NOP,

    UD_OP_ADD,
    UD_OP_SUB,
    UD_OP_DIV,
    UD_OP_MUL,
    UD_OP_MOD,
    UD_OP_NEG,

    UD_OP_EQ,
    UD_OP_GT,
    UD_OP_LT,
    UD_OP_GTEQ,
    UD_OP_LREQ,

    UD_OP_AND,
    UD_OP_OR,
    UD_OP_NOT,

    UD_OP_LSHIFT,
    UD_OP_RSHIFT,
    UD_OP_BAND,
    UD_OP_BOR,
    UD_OP_BXOR,
    UD_OP_BNOT,
} UdOp;

typedef struct ud_expr UdExpr;

struct ud_expr
{
    UdExprType type;

    union
    {
        UdVal val_;

        struct
        {
            UdOp op;
            UdExpr *lhs;
            UdExpr *rhs;
        } infix_;

        struct
        {
            UdOp op;
            UdExpr *expr;
        } prefix_, postfix_;

        struct
        {
            UdExpr *cond;
            UdExpr *then;
            UdExpr *else_;
        } if_;

        struct
        {
            UdExpr *cond;
            UdExpr *expr;
        } while_, do_;

        struct
        {
            Vec(UdExpr) exprs;
        } block_;
    };
};
