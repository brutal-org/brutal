#pragma once

#include <cc/ast/type.h>
#include <cc/ast/val.h>
#include <brutal-base>

#define CEXPR_MAX_PRECEDENCE (16)

typedef struct _CStmt CStmt;

typedef enum
{
    CEXPR_INVALID,
    CEXPR_EMPTY,

    CEXPR_SELF,
    CEXPR_CONSTANT,
    CEXPR_IDENT,

    CEXPR_PREFIX,
    CEXPR_INFIX,
    CEXPR_POSTFIX,

    CEXPR_CALL,
    CEXPR_CAST,
    CEXPR_TERNARY,
    CEXPR_INITIALIZER,
    CEXPR_LAMBDA,
    CEXPR_STATEMENT,

    CEXPR_COUNT
} CExprType;

typedef enum
{
    COP_INVALID,

    COP_INC, // ++
    COP_DEC, // --

    COP_ASSIGN,         // =
    COP_ASSIGN_ADD,     // +=
    COP_ASSIGN_SUB,     // -=
    COP_ASSIGN_MULT,    // *=
    COP_ASSIGN_DIV,     // /=
    COP_ASSIGN_MOD,     // %=
    COP_ASSIGN_BIT_AND, // &=
    COP_ASSIGN_BIT_OR,  // |=
    COP_ASSIGN_BIT_XOR, // ^=
    COP_ASSIGN_LSHIFT,  // <<=
    COP_ASSIGN_RSHIFT,  // >>=

    COP_ADD,     // +
    COP_SUB,     // -
    COP_MULT,    // *
    COP_DIV,     // /
    COP_MOD,     // mod
    COP_BIT_NOT, // ~
    COP_BIT_AND, // &
    COP_BIT_OR,  // |
    COP_BIT_XOR, // ^
    COP_LSHIFT,  // <<
    COP_RSHIFT,  // >>

    COP_NOT, // !
    COP_AND, // &&
    COP_OR,  // ||

    COP_EQ,     // ==
    COP_NOT_EQ, // !=
    COP_LT,     // <
    COP_GT,     // >
    COP_LT_EQ,  // <=
    COP_GT_EQ,  // >=

    COP_INDEX,      // []
    COP_DEREF,      // *
    COP_REF,        // &
    COP_ACCESS,     // .
    COP_PTR_ACCESS, // ->
    COP_CALL,       // (
    COP_COUNT,
} COp;

typedef struct
{
    CType type;
} CCompound;

typedef struct _CExpr CExpr;

typedef struct
{
    CExpr *designator;
    CExpr *initializer;
} CInitializer;

struct _CExpr
{
    CExprType type;
    CType sema_type;
    union
    {
        CVal constant_; /* instead of Cval holding sema_type the cexpr holds it */
        Str ident_;

        struct
        {
            COp op;
            CExpr *expr;
        } prefix_, postfix_;

        struct
        {
            COp op;
            CExpr *lhs;
            CExpr *rhs;
        } infix_;

        struct
        {
            CExpr *expr;
            Vec(CExpr) args;
        } call_;

        struct
        {
            CType type;
            CExpr *expr;
        } cast_;

        struct
        {
            CExpr *expr_cond;
            CExpr *expr_true;
            CExpr *expr_false;
        } ternary_;

        struct
        {
            CType type;
            Vec(CExpr) initializer;
        } initializer_;

        struct
        {
            CType type;
            CStmt *body;
        } lambda_;
    };
};

COp str_to_cop(Str str);

Str cop_to_str(COp type);

Str cexpr_type_to_str(CExprType type);

int cop_pre(COp cop);

int cexpr_pre(CExpr *expr);
