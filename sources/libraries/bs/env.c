#include <brutal/log.h>
#include <bs/bs.h>
#include <bs/builtin.h>

BsExpr bs_env_def(BsExpr env, BsExpr key, BsExpr value, Alloc *alloc)
{
    return bs_pair(alloc, bs_pair(alloc, key, value), env);
}

BsExpr bs_env_def_builtin(BsExpr env, const char *name, BsBuiltin builtin, Alloc *alloc)
{
    return bs_env_def(env, bs_atom(str_cast(name)), bs_builtin(builtin), alloc);
}

BsExpr bs_env_def_syntax(BsExpr env, const char *name, BsSyntax syntax, Alloc *alloc)
{
    return bs_env_def(env, bs_atom(str_cast(name)), bs_syntax(syntax), alloc);
}

BsExpr bs_env_set(BsExpr env, BsExpr key, BsExpr value, Alloc *alloc)
{
    if (bs_eq(env, bs_nil()))
    {
        return bs_nil();
    }

    if (bs_eq(bs_llhs(env), key))
    {
        return bs_pair(alloc, bs_pair(alloc, key, value), bs_rhs(env));
    }
    else
    {
        return bs_pair(alloc, bs_lhs(env), bs_env_set(env, key, value, alloc));
    }
}

BsExpr bs_env_lookup(BsExpr env, BsExpr key)
{
    if (bs_eq(env, bs_nil()))
    {
        return bs_nil();
    }

    if (bs_eq(bs_llhs(env), key))
    {
        return bs_rlhs(env);
    }

    return bs_env_lookup(bs_rhs(env), key);
}

BsExpr bs_env_default(Alloc *alloc)
{
    BsExpr env = bs_nil();

    env = bs_env_def_syntax(env, "bs-builtin-define", bs_builtin_define, alloc);
    env = bs_env_def_syntax(env, "bs-builtin-lambda", bs_builtin_lambda, alloc);
    env = bs_env_def_syntax(env, "bs-builtin-block", bs_builtin_block, alloc);
    env = bs_env_def_syntax(env, "bs-builtin-quote", bs_builtin_quote, alloc);

    return env;
}
