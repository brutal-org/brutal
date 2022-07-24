#pragma once

#include <cc/ast/decl.h>

typedef struct
{
    Vec(CDecl) decls;
    CType ret;
} CScope;

void cscope_init(CScope *self, Alloc *alloc);

void cscope_deinit(CScope *self);

bool cscope_lookup(CScope *self, Str name, CDecl *decl);

void cscope_add(CScope *self, CDecl decl);
