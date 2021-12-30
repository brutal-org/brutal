#pragma once
#include <ud/ast/expr.h>
#include <ud/ast/type.h>

typedef enum
{
    UD_STMT_DECL,
    UD_STMT_IF,
    UD_STMT_WHILE
} UdStmtType;

typedef struct
{
    Str name;
    UdExpr value;
    UdType type;
} UdVarDecl;

typedef enum
{
    UD_DECL_FUNC,
    UD_DECL_VAR,
    UD_DECL_TYPE,
} UdDeclType;

typedef struct
{
    UdStmtType type;

    union
    {
        struct
        {
            UdDeclType type;
            union
            {
                UdVarDecl var;
            };
        } decl_;
    };

} UdStmt;
