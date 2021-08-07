#include <bs/bs.h>

BsExpr bs_eval_args(BsExpr expr, BsExpr *env, Alloc *alloc)
{
    if (bs_is(expr, BS_NIL))
    {
        return bs_nil();
    }
    else
    {
        return bs_pair(
            alloc,
            bs_eval(bs_lhs(expr), env, alloc),
            bs_eval_args(bs_rhs(expr), env, alloc));
    }
}

BsExpr bs_extend_env(BsExpr params, BsExpr vals, BsExpr env, Alloc *alloc)
{
    if (bs_is(params, BS_NIL))
    {
        return env;
    }

    return bs_pair(
        alloc,
        bs_pair(alloc, bs_lhs(params), bs_lhs(vals)),
        bs_extend_env(bs_rhs(params), bs_rhs(vals), env, alloc));
}

BsExpr bs_eval(BsExpr expr, BsExpr *env, Alloc *alloc)
{
    if (bs_is_self_quoting(expr.type))
    {
        return expr;
    }

    if (bs_is(expr, BS_ATOM))
    {
        return bs_env_lookup(*env, expr);
    }

    BsExpr op = bs_eval(bs_lhs(expr), env, alloc);

    if (bs_is(op, BS_SYNTAX))
    {
        return op.syntax_(bs_rhs(expr), env, alloc);
    }

    BsExpr args = bs_eval_args(bs_rhs(expr), env, alloc);

    if (bs_is(op, BS_BUILTIN))
    {
        return op.builtin_(args, env, alloc);
    }

    if (bs_is(op, BS_LAMBDA))
    {
        BsExpr local_env = bs_extend_env(*op.lambda_.parms, args, *env, alloc);
        return bs_eval(*op.lambda_.body, &local_env, alloc);
    }

    return bs_nil();
}
