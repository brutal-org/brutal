#include <brutal/log.h>
#include <bs/bs.h>
#include <bs/builtin.h>

BsExpr bs_builtin_define(BsExpr args, BsExpr *env, Alloc *alloc)
{
    if (bs_lhs_is(args, BS_ATOM))
    {
        BsExpr key = bs_lhs(args);
        BsExpr value = bs_eval(bs_lrhs(args), env, alloc);

        *env = bs_env_def(*env, key, value, alloc);

        return bs_bool(BS_TRUE);
    }
    else if (bs_lhs_is(args, BS_PAIR))
    {
        BsExpr key = bs_llhs(args);
        BsExpr parms = bs_rlhs(args);
        BsExpr body = bs_lrhs(args);

        BsExpr lambda = bs_lambda(alloc, *env, parms, body);
        *env = bs_env_def(*env, key, lambda, alloc);

        return bs_bool(BS_TRUE);
    }
    else
    {
        return bs_nil();
    }
}

BsExpr bs_builtin_lambda(BsExpr args, BsExpr *env, Alloc *alloc)
{
    return bs_lambda(alloc, *env, bs_lhs(args), bs_lrhs(args));
}

BsExpr bs_builtin_block(BsExpr args, BsExpr *env, Alloc *alloc)
{
    BsExpr current = args;
    BsExpr result = bs_nil();

    while (bs_is(current, BS_PAIR))
    {
        result = bs_eval(bs_lhs(current), env, alloc);
        current = bs_rhs(current);
    }

    return result;
}

BsExpr bs_builtin_quote(BsExpr args, MAYBE_UNUSED BsExpr *env, MAYBE_UNUSED Alloc *alloc)
{
    return args;
}
