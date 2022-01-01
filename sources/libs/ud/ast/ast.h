#pragma once
#include <ud/ast/expr.h>
#include <ud/ast/stmt.h>

typedef enum
{
    UD_NODE_STMT,
    UD_NODE_EXPR,
} UdAstNodeType;

typedef struct ast_node
{
    UdAstNodeType type;

    union
    {
        UdExpr expr;
        UdStmt stmt;
    };

} UdAstNode;

typedef Vec(UdAstNode) UdAst;
