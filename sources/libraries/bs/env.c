#include <bs/bs.h>

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

    if (bs_eq(bs_caar(env), key))
    {
        return bs_pair(alloc, bs_pair(alloc, key, value), bs_cdr(env));
    }
    else
    {
        return bs_pair(alloc, bs_car(env), bs_env_set(env, key, value, alloc));
    }
}

BsExpr bs_env_lookup(BsExpr env, BsExpr key)
{
    if (bs_eq(bs_caar(env), bs_nil()))
    {
        return bs_nil();
    }

    if (bs_eq(bs_caar(env), key))
    {
        return bs_cadr(env);
    }

    return bs_env_lookup(bs_cdr(env), key);
}
