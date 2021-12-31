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

UdDecl sema_find_reference(UdScope *self, Str name)
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

    return true;
}

void sema_add(UdSema *self, UdDecl decl)
{
    vec_push(&(vec_end(&self->scopes)->decls), decl);
}

void sema_analyze_node(UdSema *self, UdAstNode node)
{
    if (node.type == UD_NODE_EXPR)
    {
        sema_analyze_expr(self, node.expr);
    }

    if (node.type == UD_NODE_STMT)
    {
        if (node.stmt.type == UD_STMT_DECL)
        {
            if (node.stmt.decl_.type == UD_DECL_VAR)
            {
                if (sema_analyze_expr(self, node.stmt.decl_.var.value))
                    sema_add(self, node.stmt.decl_);
            }
        }
    }
}

void ud_sema_analyze(UdSema *self, UdAst *ast)
{
    vec_foreach(node, ast)
    {
        sema_analyze_node(self, *node);
    }
}
