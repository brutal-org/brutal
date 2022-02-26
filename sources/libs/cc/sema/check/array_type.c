#include <cc/sema/check/array_type.h>
#include <cc/sema/check/utils.h>

void sema_array_check_expr(CSema *self, CExpr *cexpr, MAYBE_UNUSED void *ctx)
{
    if (cexpr->type != CEXPR_INFIX || cexpr->infix_.op != COP_INDEX)
    {
        return;
    }

    CType left_type = cexpr->infix_.lhs->sema_type;

    if (left_type.type != CTYPE_ARRAY && left_type.type != CTYPE_PTR)
    {
        int id = csema_report$(self, CSEMA_ERR, *cexpr, "Trying to use an array operator with a non ptr/array object");

        Str expression_type_dump = csema_ctype_to_str(*cexpr->infix_.lhs, self->alloc);

        csema_comment$(self, id, *cexpr->infix_.lhs, "Expression with type: {}", expression_type_dump);

        return;
    }
}
