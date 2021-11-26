#include <cc/sema/sema.h>

void csema_init(CSema *self, Alloc *alloc)
{
    *self = (CSema){};
    self->alloc = alloc;
    vec_init(&self->scopes, alloc);
}

void csema_deinit(CSema *self)
{
    vec_deinit(&self->scopes);
}

void csema_scope_enter(CSema *self)
{
    CScope scope;
    cscope_init(&scope, self->alloc);
    vec_push(&self->scopes, scope);
}

void csema_scope_leave(CSema *self)
{
    CScope scope = vec_pop(&self->scopes);
    cscope_deinit(&scope);
}

void csema_scope_add(CSema *self, CDecl decl)
{
    CScope *scope = vec_end(&self->scopes);
    cscope_add(scope, decl);
}
