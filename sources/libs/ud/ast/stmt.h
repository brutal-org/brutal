#pragma once
#include <ud/ast/expr.h>
#include <ud/ast/type.h>

typedef enum
{
    UD_STMT_DECL,
    UD_STMT_IF,
    UD_STMT_WHILE
} UdStmtType;

typedef struct ast_node UdAstNode;

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
    Str name;
    UdType type;
} UdFuncParam;

typedef struct
{
    Vec(UdAstNode) body;
    Vec(UdFuncParam) params;

    UdType return_type;

} UdFuncDecl;

typedef struct
{

    UdDeclType type;

    Str name;

    union
    {
        UdVarDecl var;
        UdFuncDecl func;
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
