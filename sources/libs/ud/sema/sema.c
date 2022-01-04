#include <brutal/debug.h>
#include <ud/sema/sema.h>

void sema_enter_scope(UdSema *self);

void ud_sema_init(UdSema *self, Alloc *alloc)
{
    *self = (UdSema){};
    self->alloc = alloc;

    vec_init(&self->scopes, alloc);

    sema_enter_scope(self);
}

void sema_enter_scope(UdSema *self)
{
    UdScope scope = {};

    vec_push(&self->scopes, scope);

    vec_init(&(vec_end(&self->scopes)->decls), self->alloc);
}

void sema_scope_leave(UdSema *self)
{
    UdScope scope = vec_pop(&self->scopes);

    vec_deinit(&scope.decls);
}

static UdDecl sema_find_reference(UdScope *self, Str name)
{

    vec_foreach(decl, &self->decls)
    {
        if (str_eq(decl->name, name))
        {
            return *decl;
        }
    }

    return (UdDecl){};
}

static void sema_add(UdSema *self, UdDecl decl);

bool sema_analyze_expr(UdSema *self, UdExpr expr)
{
    if (expr.type == UD_EXPR_REFERENCE)
    {
        if (sema_find_reference(vec_end(&self->scopes), expr.reference).type == UD_DECL_NONE)
        {
            log$("Error: unbound value `{}`", expr.reference);

            return false;
        }
    }

    if (expr.type == UD_EXPR_DECL)
    {
        if (expr.decl.type == UD_DECL_VAR)
        {
            if (sema_analyze_expr(self, *expr.decl.var.value))
            {
                sema_add(self, expr.decl);
            }
        }
    }

    return true;
}

static void sema_add(UdSema *self, UdDecl decl)
{
    vec_push(&(vec_end(&self->scopes)->decls), decl);
}

static void sema_analyze_node(UdSema *self, UdAstNode node)
{
    sema_analyze_expr(self, node.expr);
}

void ud_sema_analyze(UdSema *self, UdAst *ast)
{
    vec_foreach(node, ast)
    {
        sema_analyze_node(self, *node);
    }
}
