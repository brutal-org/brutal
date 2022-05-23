#include <brutal-debug>
#include <cc/trans.h>

#include "cc/builder.h"

static void ctrans_op_fix(Emit *emit, COp op)
{
    emit_fmt$(emit, "{}", cop_to_str(op));
}

static void ctrans_expr_pre(Emit *emit, CExpr expr, int parent_pre)
{
    int pre = cexpr_pre(&expr);

    if (pre >= parent_pre)
    {
        emit_fmt$(emit, "(");
    }

    switch (expr.type)
    {
    case CEXPR_EMPTY:
        break;

    case CEXPR_CONSTANT:
        ctrans_value(emit, expr.constant_);
        break;

    case CEXPR_IDENT:
        emit_fmt$(emit, "{}", expr.ident_);
        break;

    case CEXPR_SELF:
        break;

    case CEXPR_PREFIX:
        ctrans_op_fix(emit, expr.prefix_.op);

        if (expr.prefix_.expr != nullptr)
        {
            ctrans_expr_pre(emit, *expr.prefix_.expr, pre);
        }
        break;

    case CEXPR_POSTFIX:
        if (expr.postfix_.expr != nullptr)
        {
            ctrans_expr_pre(emit, *expr.postfix_.expr, pre);
        }
        ctrans_op_fix(emit, expr.postfix_.op);
        break;

    case CEXPR_INFIX:
        ctrans_expr_pre(emit, *expr.infix_.lhs, pre);
        ctrans_op_fix(emit, expr.infix_.op);
        ctrans_expr_pre(emit, *expr.infix_.rhs, pre);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt$(emit, "]");
        }
        break;

    case CEXPR_CALL:
        ctrans_expr_pre(emit, *expr.call_.expr, pre);
        emit_fmt$(emit, "(");
        bool first = true;
        vec_foreach_v(v, &expr.call_.args)
        {
            if (first)
            {
                first = !first;
            }
            else
            {
                emit_fmt$(emit, ", ");
            }

            ctrans_expr_pre(emit, v, CEXPR_MAX_PRECEDENCE);
        }
        emit_fmt$(emit, ")");
        break;

    case CEXPR_CAST:
        emit_fmt$(emit, "(");
        ctrans_type(emit, expr.cast_.type);
        emit_fmt$(emit, ")");
        ctrans_expr_pre(emit, *expr.cast_.expr, pre);
        break;

    case CEXPR_TERNARY:
        ctrans_expr_pre(emit, *expr.ternary_.expr_cond, pre);
        emit_fmt$(emit, " ? ");
        ctrans_expr_pre(emit, *expr.ternary_.expr_true, pre);
        emit_fmt$(emit, " : ");
        ctrans_expr_pre(emit, *expr.ternary_.expr_false, pre);
        break;

    case CEXPR_INITIALIZER:
        emit_fmt$(emit, "(");
        ctrans_type(emit, expr.initializer_.type);
        emit_fmt$(emit, ")");

        emit_fmt$(emit, "{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &expr.initializer_.initializer)
        {
            ctrans_expr_pre(emit, v, CEXPR_MAX_PRECEDENCE);
            emit_fmt$(emit, ",\n");
        }

        emit_deident(emit);
        emit_fmt$(emit, "}}");
        break;

    case CEXPR_LAMBDA:
        emit_fmt$(emit, "[]");
        ctrans_func_params(emit, expr.lambda_.type);
        ctrans_stmt(emit, *expr.lambda_.body);
        break;

    default:
        panic$("Unknown cexpr type {}", expr.type);
    }

    if (pre >= parent_pre)
    {
        emit_fmt$(emit, ")");
    }
}

void ctrans_expr(Emit *emit, CExpr expr)
{
    ctrans_expr_pre(emit, expr, CEXPR_MAX_PRECEDENCE);
}
