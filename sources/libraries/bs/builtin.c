#include <brutal/log.h>
#include <bs/bs.h>
#include <bs/builtin.h>

BsExpr bs_builtin_define(BsExpr args, BsExpr *env, Alloc *alloc)
{
    if (bs_car_is(args, BS_VAL_ATOM))
    {
        BsExpr key = bs_car(args);
        BsExpr value = bs_eval(bs_cadr(args), env, alloc);

        *env = bs_env_def(*env, key, value, alloc);

        return bs_bool(BS_TRUE);
    }
    else if (bs_car_is(args, BS_VAL_PAIR))
    {
        BsExpr key = bs_caar(args);
        BsExpr parms = bs_cdar(args);
        BsExpr body = bs_cadr(args);

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
    return bs_lambda(alloc, *env, bs_car(args), bs_cadr(args));
}

BsExpr bs_builtin_block(BsExpr args, BsExpr *env, Alloc *alloc)
{
    BsExpr current = args;
    BsExpr result = bs_nil();

    while (bs_is(current, BS_VAL_PAIR))
    {
        result = bs_eval(bs_car(current), env, alloc);
        current = bs_cdr(current);
    }

    return result;
}
