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
    UdExpr value;
    UdType type;
} UdVarDecl;

typedef enum
{
    UD_DECL_NONE,
    UD_DECL_FUNC,
    UD_DECL_VAR,
    UD_DECL_TYPE,
} UdDeclType;

typedef struct
{

    UdDeclType type;

    Str name;

    union
    {
        UdVarDecl var;
    };

} UdDecl;

typedef struct
{
    UdStmtType type;

    union
    {
        UdDecl decl_;
    };

} UdStmt;
