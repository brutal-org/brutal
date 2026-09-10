#include <brutal-fmt>
#include <cc/sema/passes.h>
#include <cc/trans.h>

void cpass_scope_propagation(CSema *ctx, int what, void *node)
{
    bool is_scope =
        cvisit_node_is(node, CVISIT_DECL, CDECL_FUNC) &&
        cvisit_node_is(node, CVISIT_STMT, CSTMT_BLOCK) &&
        cvisit_node_is(node, CVISIT_UNIT, 0);

    if (!is_scope)
        return;

    if (what & CVISIT_BEGIN)
    {
        if (cvisit_node_is(node, CVISIT_DECL, CDECL_FUNC))
        {
            csema_scope_enter_func(ctx, ((CDecl *)node)->func_.type);
        }
        else
        {
            csema_scope_enter(ctx);
        }
    }
    else
    {
        csema_scope_leave(ctx);
    }
}

void cpass_types_propagation(MAYBE_UNUSED CSema *ctx, MAYBE_UNUSED int what, MAYBE_UNUSED void *node)
{

    if (what & CVISIT_BEGIN)
    {
    }
    else
    {
    }
}

void cpass_op_array(CSema *ctx, int, void *node)
{
    CExpr *expr = node;

    if (expr->type != CEXPR_INFIX || expr->infix_.op != COP_INDEX)
    {
        return;
    }

    CType lhs = expr->infix_.lhs->sema_type;

    if (lhs.type != CTYPE_ARRAY && lhs.type != CTYPE_PTR)
    {
        int id = csema_report$(ctx, PARSE_ERR, *expr, "Trying to use an array operator with a non ptr/array object");

        Str str = ctrans_type_str(*expr->infix_.lhs, ctx->alloc);
        csema_comment$(ctx, id, *expr->infix_.lhs, "Expression with type: {}", str);
    }
}

void cpass_op_deref(CSema *ctx, int, void *node)
{
    CExpr *expr = node;

    if (expr->type != CEXPR_PREFIX || expr->prefix_.op != COP_DEREF)
    {
        return;
    }

    CType left_type = expr->prefix_.expr->sema_type;

    if (left_type.type != CTYPE_ARRAY && left_type.type != CTYPE_PTR)
    {
        int id = csema_report$(ctx, PARSE_ERR, *expr, "Trying to use an deref operator with a non ptr/array object");

        Str str = ctrans_type_str(*expr->prefix_.expr, ctx->alloc);
        csema_comment$(ctx, id, *expr->prefix_.expr, "Expression with type: {}", str);
    }
}

void cpass_expr_call(CSema *ctx, int, void *node)
{
    CExpr *expr = node;

    if (expr->type != CEXPR_CALL)
    {
        return;
    }

    CType left_type = expr->call_.expr->sema_type;

    if (left_type.type != CTYPE_FUNC)
    {
        int id = csema_report$(ctx, PARSE_ERR, *expr, "calling a non callable expression");

        Str str = ctrans_type_str(*expr->call_.expr, ctx->alloc);
        csema_comment$(ctx, id, *expr->call_.expr, "Expression with type: {}", str);
    }
}

void cpass_decl_def(CSema *ctx, int, void *node)
{
    CDecl *decl = node;

    CDecl other;
    if (csema_lookup_current_scope(ctx, decl->name, &other))
    {
        int id = csema_report$(ctx, PARSE_ERR, *decl, "redefinition of: {}", decl->name);
        csema_comment$(ctx, id, other, "already defined here");
    }
}

void cpass_decl_deref(CSema *ctx, int, void *node)
{
    CExpr *expr = node;

    CDecl prev;
    if (expr->type == CEXPR_IDENT && !csema_lookup(ctx, expr->ident_, &prev))
    {
        csema_report$(ctx, PARSE_ERR, *expr, "Not defined symbol: {}", expr->ident_);
    }
}

static CVisit passes[] = {
    {},
};

CVisites cpass_all(void)
{
    return slice_array$(CVisites, passes);
}
