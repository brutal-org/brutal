#include <ud/ud.h>

UdExpr ud_eval_args(UdExpr expr, UdExpr *env, Alloc *alloc)
{
    if (ud_is(expr, UD_NIL))
    {
        return ud_nil();
    }
    else
    {
        return ud_pair(
            alloc,
            ud_eval(ud_lhs(expr), env, alloc),
            ud_eval_args(ud_rhs(expr), env, alloc));
    }
}

UdExpr ud_extend_env(UdExpr params, UdExpr vals, UdExpr env, Alloc *alloc)
{
    if (ud_is(params, UD_NIL))
    {
        return env;
    }

    return ud_pair(
        alloc,
        ud_pair(alloc, ud_lhs(params), ud_lhs(vals)),
        ud_extend_env(ud_rhs(params), ud_rhs(vals), env, alloc));
}

UdExpr ud_eval(UdExpr expr, UdExpr *env, Alloc *alloc)
{
    if (ud_is_self_quoting(expr.type))
    {
        return expr;
    }

    if (ud_is(expr, UD_ATOM))
    {
        return ud_env_lookup(*env, expr);
    }

    UdExpr op = ud_eval(ud_lhs(expr), env, alloc);

    if (ud_is(op, UD_SYNTAX))
    {
        return op.syntax_(ud_rhs(expr), env, alloc);
    }

    UdExpr args = ud_eval_args(ud_rhs(expr), env, alloc);

    if (ud_is(op, UD_BUILTIN))
    {
        return op.builtin_(args, env, alloc);
    }

    if (ud_is(op, UD_LAMBDA))
    {
        UdExpr local_env = ud_extend_env(*op.lambda_.parms, args, *env, alloc);
        return ud_eval(*op.lambda_.body, &local_env, alloc);
    }

    return ud_nil();
}
