#pragma once
#include <ud/ast.h>
#include <ud/parse/parse.h>

typedef struct
{
    Vec(UdDecl) decls;
} UdScope;

typedef struct
{

    Vec(UdScope) scopes;
    Alloc *alloc;
} UdSema;

void ud_sema_init(UdSema *self, Alloc *alloc);
void ud_sema_analyze(UdSema *self, UdAst *ast);
