#pragma once
#include <ud/ast/expr.h>

typedef struct ast_node
{
    UdExpr expr;
} UdAstNode;

typedef Vec(UdAstNode) UdAst;
