#include <cc/builder.h>
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

CDecl csema_lookup(CSema *self, Str name)
{
    vec_foreach(scope, &self->scopes)
    {
        CDecl d = cscope_lookup(scope, name);
        if (d.type != CDECL_NIL)

        {
            return d;
        }
    }
    return (CDecl){};
}

CType csema_scope_return_type(CSema *self)
{
    vec_foreach(scope, &self->scopes)
    {
        if (scope->expected_result.type != CTYPE_INVALID)
        {
            return scope->expected_result;
        }
    }
    return (CType){.type = CTYPE_INVALID};
}

void csema_scope_enter_func(CSema *self, CType func_type)
{
    CScope scope;
    cscope_init(&scope, self->alloc);
    scope.expected_result = *func_type.func_.ret;

    // add each argument in scope
    vec_foreach(arg, &func_type.func_.params)
    {
        CDecl decl = (CDecl){
            .type = CDECL_VAR,
            .name = str_dup(arg->name, self->alloc),
            .var_ = {
                .type = arg->type,
                .expr = cexpr_empty(),
            },
        };
        cscope_add(&scope, decl);
    }
    vec_push(&self->scopes, scope);
}
