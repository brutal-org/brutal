#include <brutal/debug.h>
#include <cc/trans.h>

static void cgen_c_op_fix(Emit *emit, COp op)
{
    emit_fmt(emit, "{}", cop_to_str(op));
}

static void cgen_c_expr_pre(Emit *emit, CExpr expr, int parent_pre)
{
    int pre = cexpr_pre(&expr);

    if (pre > parent_pre)
    {
        emit_fmt(emit, "(");
    }

    switch (expr.type)
    {
    case CEXPR_EMPTY:
        break;

    case CEXPR_CONSTANT:
        cgen_c_value(emit, expr.constant_);
        break;

    case CEXPR_IDENT:
        emit_fmt(emit, "{}", expr.ident_);
        break;

    case CEXPR_SELF:
        break;

    case CEXPR_PREFIX:
        cgen_c_op_fix(emit, expr.prefix_.op);

        if (expr.prefix_.expr != nullptr)
        {
            cgen_c_expr_pre(emit, *expr.prefix_.expr, pre);
        }
        break;

    case CEXPR_POSTFIX:
        if (expr.postfix_.expr != nullptr)
        {
            cgen_c_expr_pre(emit, *expr.postfix_.expr, pre);
        }
        cgen_c_op_fix(emit, expr.postfix_.op);
        break;

    case CEXPR_INFIX:
        cgen_c_expr_pre(emit, *expr.infix_.lhs, pre);
        cgen_c_op_fix(emit, expr.infix_.op);
        cgen_c_expr_pre(emit, *expr.infix_.rhs, pre);
        if (expr.infix_.op == COP_INDEX)
        {
            emit_fmt(emit, "]");
        }
        break;

    case CEXPR_CALL:
        cgen_c_expr_pre(emit, *expr.call_.expr, pre);
        emit_fmt(emit, "(");
        bool first = true;
        vec_foreach_v(v, &expr.call_.args)
        {
            if (first)
            {
                first = !first;
            }
            else
            {
                emit_fmt(emit, ", ");
            }

            cgen_c_expr_pre(emit, v, CEXPR_MAX_PRECEDENCE);
        }
        emit_fmt(emit, ")");
        break;

    case CEXPR_CAST:
        emit_fmt(emit, "(");
        cgen_c_type(emit, expr.cast_.type);
        emit_fmt(emit, ")");
        cgen_c_expr_pre(emit, *expr.cast_.expr, pre);
        break;

    case CEXPR_TERNARY:
        cgen_c_expr_pre(emit, *expr.ternary_.expr_cond, pre);
        emit_fmt(emit, " ? ");
        cgen_c_expr_pre(emit, *expr.ternary_.expr_true, pre);
        emit_fmt(emit, " : ");
        cgen_c_expr_pre(emit, *expr.ternary_.expr_false, pre);
        break;

    case CEXPR_INITIALIZER:
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &expr.initializer_.initializer)
        {
            cgen_c_expr_pre(emit, v, CEXPR_MAX_PRECEDENCE);
            emit_fmt(emit, ",\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
        break;

    default:
        panic$("Unknown cexpr type {}", expr.type);
    }

    if (pre > parent_pre)
    {
        emit_fmt(emit, ")");
    }
}

void cgen_c_expr(Emit *emit, CExpr expr)
{
    cgen_c_expr_pre(emit, expr, CEXPR_MAX_PRECEDENCE);
}
