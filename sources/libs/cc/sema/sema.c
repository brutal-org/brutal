#include <brutal/debug.h>
#include <cc/builder.h>
#include <cc/sema/sema.h>

void csema_init(CSema *self, Alloc *alloc)
{
    *self = (CSema){};
    self->current_pass = str$("initial");
    self->alloc = alloc;
    vec_init(&self->scopes, alloc);
    parse_reports_init(&self->reports, alloc);
}

void csema_scope_reset(CSema *self)
{
    self->scopes.len = 0;
}

void csema_deinit(CSema *self)
{
    vec_deinit(&self->scopes);
    parse_reports_deinit(&self->reports);
}

void csema_scope_enter(CSema *self)
{
    CScope scope = {};
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
    CScope *scope = &vec_last(&self->scopes);
    cscope_add(scope, decl);
}

bool csema_lookup(CSema *self, Str name, CDecl *decl)
{
    vec_foreach_rev(scope, &self->scopes)
    {
        if (cscope_lookup(scope, name, decl))
        {
            return true;
        }
    }

    return false;
}

bool csema_lookup_current_scope(CSema *self, Str name, CDecl *decl)
{
    CScope *scope = &vec_last(&self->scopes);
    return cscope_lookup(scope, name, decl);
}

CType csema_scope_return_type(CSema *self)
{
    vec_foreach_rev(scope, &self->scopes)
    {
        if (scope->ret.type != CTYPE_INVALID)
        {
            return scope->ret;
        }
    }

    return (CType){.type = CTYPE_INVALID};
}

void csema_scope_enter_func(CSema *self, CType func_type)
{
    CScope scope;
    cscope_init(&scope, self->alloc);
    scope.ret = *func_type.func_.ret;

    vec_foreach(arg, &func_type.func_.params)
    {
        CDecl decl = cdecl_var(arg->name, arg->type, cexpr_empty());
        cscope_add(&scope, decl);
    }

    vec_push(&self->scopes, scope);
}
