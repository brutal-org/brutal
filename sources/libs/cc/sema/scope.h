#pragma once

#include <cc/ast/decl.h>

typedef struct
{
    Vec(CDecl) decls;
    CType expected_result;
} CScope;

void cscope_init(CScope *self, Alloc *alloc);

void cscope_deinit(CScope *self);

CDecl cscope_lookup(CScope *self, Str name);

void cscope_add(CScope *self, CDecl decl);
