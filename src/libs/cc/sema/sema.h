#pragma once

#include <cc/sema/scope.h>

typedef struct
{
    Vec(CScope) scopes;
    Alloc *alloc;
} CSema;

void csema_init(CSema *self, Alloc *alloc);

void csema_deinit(CSema *self);

void csema_scope_enter(CSema *self);

void csema_scope_enter_func(CSema *self, CType func_type);

void csema_scope_leave(CSema *self);

void csema_scope_add(CSema *self, CDecl decl);

CDecl csema_lookup(CSema *self, Str name);

CType csema_scope_return_type(CSema *self);
