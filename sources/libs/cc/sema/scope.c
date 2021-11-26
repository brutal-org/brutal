#include <cc/sema/scope.h>

void cscope_init(CScope *self, Alloc *alloc)
{
    *self = (CScope){};
    vec_init(&self->decls, alloc);
}

void cscope_deinit(CScope *self)
{
    vec_deinit(&self->decls);
}

CDecl cscope_lookup(CScope *self, Str name)
{
    for (CDecl *decl = vec_begin(&self->decls); decl != vec_end(&self->decls); ++decl)
    {
        if (str_eq(decl->name, name))
            return *decl;
    }

    return (CDecl){};
}

void cscope_add(CScope *self, CDecl decl)
{
    cseme_scope_enter(&self->decls);
    vec_push(&self->decls, decl);
}
