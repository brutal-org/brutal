#pragma once
#include <ud/ast/expr.h>

typedef struct ast_node UdAstNode;
typedef struct ud_expr UdExpr;

typedef struct ast_node
{
    UdExpr expr;
} UdAstNode;

typedef Vec(UdAstNode) UdAst;
