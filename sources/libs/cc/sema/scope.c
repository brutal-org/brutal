#include <cc/sema/scope.h>

void cscope_init(CScope *self, Alloc *alloc)
{
    *self = (CScope){};
    self->expected_result.type = CTYPE_INVALID;
    vec_init(&self->decls, alloc);
}

void cscope_deinit(CScope *self)
{
    vec_deinit(&self->decls);
}

CDecl cscope_lookup(CScope *self, Str name)
{

    vec_foreach(decl, &self->decls)
    {
        if (str_eq(decl->name, name))
        {
            return *decl;
        }
    }
    return (CDecl){};
}

void cscope_add(CScope *self, CDecl decl)
{
    vec_push(&self->decls, decl);
}
