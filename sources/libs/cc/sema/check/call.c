#include <cc/sema/check/call.h>
#include <cc/sema/check/utils.h>

void sema_call_check_expr(CSema *self, CExpr *cexpr, MAYBE_UNUSED void *ctx)
{
    if (cexpr->type != CEXPR_CALL)
    {
        return;
    }
    CType left_type = cexpr->call_.expr->sema_type;

    if (left_type.type != CTYPE_FUNC)
    {
        int id = csema_report$(self, PARSE_ERR, *cexpr, "calling a non callable expression");
        Str expression_type_dump = csema_ctype_to_str(*cexpr->call_.expr, self->alloc);
        csema_comment$(self, id, *cexpr->call_.expr, "Expression with type: {}", expression_type_dump);
    }
}
