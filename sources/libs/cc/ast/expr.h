#pragma once

#include <brutal/base.h>
#include <cc/ast/type.h>
#include <cc/ast/val.h>

typedef enum
{
    CEXPR_INVALID,
    CEXPR_EMPTY,

    CEXPR_CONSTANT,
    CEXPR_IDENTIFIER,

    CEXPR_PREFIX,
    CEXPR_INFIX,
    CEXPR_POSTFIX,

    CEXPR_CALL,
    CEXPR_CAST,
    CEXPR_TERNARY,
    CEXPR_ARRAY_INITIALIZER,  // { [0] = 1, [1] = 2, }
    CEXPR_STRUCT_INITIALIZER, // { .a = 1, .b = 2, }
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
} COp;

typedef struct
{
    CType type;
} CCompound;

typedef struct cexpr CExpr;

typedef struct
{
    CExpr *designator;
    CExpr *initializer;
} CInitializer;

struct cexpr
{
    CExprType type;

    union
    {
        CVal constant_;
        Str identifier_;

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
            Vec(CInitializer) initializer;
        } array_initializer_, struct_initializer_;
    };
};

Str cop_to_str(COp type);

int cop_precedence(COp cop);
