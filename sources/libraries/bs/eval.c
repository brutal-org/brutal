#include <bs/bs.h>

BsExpr bs_eval_args(BsExpr expr, BsExpr *env, Alloc *alloc)
{
    if (bs_is(expr, BS_VAL_NIL))
    {
        return bs_nil();
    }
    else
    {
        return bs_pair(
            alloc,
            bs_eval(bs_car(expr), env, alloc),
            bs_eval_args(bs_cdr(expr), env, alloc));
    }
}

BsExpr bs_extend_env(BsExpr params, BsExpr vals, BsExpr env, Alloc *alloc)
{
    if (bs_is(params, BS_VAL_NIL))
    {
        return env;
    }

    return bs_pair(
        alloc,
        bs_pair(alloc, bs_car(params), bs_car(vals)),
        bs_extend_env(bs_cdr(params), bs_cdr(vals), env, alloc));
}

BsExpr bs_eval(BsExpr expr, BsExpr *env, Alloc *alloc)
{
    if (bs_is_self_quoting(expr.type))
    {
        return expr;
    }

    if (bs_is(expr, BS_VAL_ATOM))
    {
        return bs_env_lookup(*env, expr);
    }

    BsExpr op = bs_eval(bs_car(expr), env, alloc);

    if (bs_is(op, BS_VAL_SYNTAX))
    {
        return op.syntax_(bs_cdr(expr), env, alloc);
    }

    BsExpr args = bs_eval_args(bs_cdr(expr), env, alloc);

    if (bs_is(op, BS_VAL_BUILTIN))
    {
        return op.builtin_(args, env, alloc);
    }

    BsExpr local_env = bs_extend_env(*op.lambda_.parms, args, *env, alloc);

    return bs_eval(*op.lambda_.body, &local_env, alloc);
}
