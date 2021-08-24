#include <brutal/log.h>
#include <udfore/builtin.h>
#include <udfore/udfore.h>

UdExpr ud_builtin_define(UdExpr args, UdExpr *env, Alloc *alloc)
{
    if (ud_lhs_is(args, UD_ATOM))
    {
        UdExpr key = ud_lhs(args);
        UdExpr value = ud_eval(ud_lrhs(args), env, alloc);

        *env = ud_env_def(*env, key, value, alloc);

        return ud_bool(UD_TRUE);
    }
    else if (ud_lhs_is(args, UD_PAIR))
    {
        UdExpr key = ud_llhs(args);
        UdExpr parms = ud_rlhs(args);
        UdExpr body = ud_lrhs(args);

        UdExpr lambda = ud_lambda(alloc, *env, parms, body);
        *env = ud_env_def(*env, key, lambda, alloc);

        return ud_bool(UD_TRUE);
    }
    else
    {
        return ud_nil();
    }
}

UdExpr ud_builtin_lambda(UdExpr args, UdExpr *env, Alloc *alloc)
{
    return ud_lambda(alloc, *env, ud_lhs(args), ud_lrhs(args));
}

UdExpr ud_builtin_block(UdExpr args, UdExpr *env, Alloc *alloc)
{
    UdExpr current = args;
    UdExpr result = ud_nil();

    while (ud_is(current, UD_PAIR))
    {
        result = ud_eval(ud_lhs(current), env, alloc);
        current = ud_rhs(current);
    }

    return result;
}

UdExpr ud_builtin_if(UdExpr args, UdExpr *env, Alloc *alloc)
{
    UdExpr result = ud_nil();
    UdExpr cond = ud_eval(ud_lhs(args), env, alloc);

    if (ud_is_truth(cond))
    {
        result = ud_eval(ud_lrhs(args), env, alloc);
    }
    else if (ud_is_atom(ud_lrrhs(args), str$("else")))
    {
        result = ud_eval(ud_lrrrhs(args), env, alloc);
    }

    return result;
}

UdExpr ud_builtin_while(UdExpr args, UdExpr *env, Alloc *alloc)
{
    bool any = false;
    UdExpr result = ud_bool(false);
    UdExpr cond = ud_eval(ud_lhs(args), env, alloc);

    while (ud_is_truth(cond))
    {
        any = true;
        result = ud_eval(ud_lrhs(args), env, alloc);
        cond = ud_eval(ud_lhs(args), env, alloc);
    }

    if (!any && ud_is_atom(ud_lrrhs(args), str$("else")))
    {
        result = ud_eval(ud_lrrrhs(args), env, alloc);
    }

    return result;
}

UdExpr ud_builtin_quote(UdExpr args, MAYBE_UNUSED UdExpr *env, MAYBE_UNUSED Alloc *alloc)
{
    return args;
}

UdExpr ud_builtin_pair(UdExpr args, MAYBE_UNUSED UdExpr *env, Alloc *alloc)
{
    return ud_pair(alloc, ud_lhs(args), ud_lrhs(args));
}
