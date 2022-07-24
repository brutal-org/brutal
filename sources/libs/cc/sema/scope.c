#include <cc/sema/scope.h>

void cscope_init(CScope *self, Alloc *alloc)
{
    *self = (CScope){};
    self->ret.type = CTYPE_INVALID;
    vec_init(&self->decls, alloc);
}

void cscope_deinit(CScope *self)
{
    vec_deinit(&self->decls);
}

bool cscope_lookup(CScope *self, Str name, CDecl *out_decl)
{
    vec_foreach_rev(decl, &self->decls)
    {
        if (str_eq(decl->name, name))
        {
            *out_decl = *decl;
            return true;
        }
    }

    return false;
}

void cscope_add(CScope *self, CDecl decl)
{
    vec_push(&self->decls, decl);
}
