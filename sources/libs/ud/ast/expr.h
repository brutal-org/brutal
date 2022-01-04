#pragma once

#include <brutal/ds.h>
#include <ud/ast/decl.h>
#include <ud/ast/expr.h>
#include <ud/ast/type.h>
#include <ud/ast/val.h>

// ------------- Enums -----------
typedef enum
{
    UD_EXPR_NIL,
    UD_EXPR_CONSTANT,
    UD_EXPR_BINOP,
    UD_EXPR_FUNC_CALL,
    UD_EXPR_CONDITION,
    UD_EXPR_REFERENCE,
    UD_EXPR_DECL,

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

// ------------- Structs -----------
typedef struct
{
    Str name;
    Vec(UdExpr) params;
} UdFuncCall;

typedef struct
{
    UdExpr *left;
    UdExpr *right;
    UdOp op;
} UdBinOp;

struct ud_expr
{
    UdExprType type;

    union
    {
        UdVal const_;

        Str reference;

        UdBinOp bin_op;

        UdFuncCall func_call;

        UdDecl decl;
    };
};
