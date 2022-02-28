#include <cc/sema/check/deref_type.h>
#include <cc/sema/check/utils.h>

void sema_deref_check_expr(CSema *self, CExpr *cexpr, MAYBE_UNUSED void *ctx)
{
    if (cexpr->type != CEXPR_PREFIX || cexpr->prefix_.op != COP_DEREF)
    {
        return;
    }
    CType left_type = cexpr->prefix_.expr->sema_type;

    if (left_type.type != CTYPE_ARRAY && left_type.type != CTYPE_PTR)
    {
        int id = csema_report$(self, PARSE_ERR, *cexpr, "Trying to use an deref operator with a non ptr/array object");

        Str expression_type_dump = csema_ctype_to_str(*cexpr->prefix_.expr, self->alloc);

        csema_comment$(self, id, *cexpr->prefix_.expr, "Expression with type: {}", expression_type_dump);

        return;
    }
}
