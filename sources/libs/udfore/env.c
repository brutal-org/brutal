#include <brutal/log.h>
#include <udfore/builtin.h>
#include <udfore/udfore.h>

UdExpr ud_env_def(UdExpr env, UdExpr key, UdExpr value, Alloc *alloc)
{
    return ud_pair(alloc, ud_pair(alloc, key, value), env);
}

UdExpr ud_env_def_builtin(UdExpr env, const char *name, UdBuiltin builtin, Alloc *alloc)
{
    return ud_env_def(env, ud_atom(str$(name)), ud_builtin(builtin), alloc);
}

UdExpr ud_env_def_syntax(UdExpr env, const char *name, UdSyntax syntax, Alloc *alloc)
{
    return ud_env_def(env, ud_atom(str$(name)), ud_syntax(syntax), alloc);
}

UdExpr ud_env_set(UdExpr env, UdExpr key, UdExpr value, Alloc *alloc)
{
    if (ud_eq(env, ud_nil()))
    {
        return ud_nil();
    }

    if (ud_eq(ud_llhs(env), key))
    {
        return ud_pair(alloc, ud_pair(alloc, key, value), ud_rhs(env));
    }
    else
    {
        return ud_pair(alloc, ud_lhs(env), ud_env_set(env, key, value, alloc));
    }
}

UdExpr ud_env_lookup(UdExpr env, UdExpr key)
{
    if (ud_eq(env, ud_nil()))
    {
        return ud_nil();
    }

    if (ud_eq(ud_llhs(env), key))
    {
        return ud_rlhs(env);
    }

    return ud_env_lookup(ud_rhs(env), key);
}

UdExpr ud_env_default(Alloc *alloc)
{
    UdExpr env = ud_nil();

    env = ud_env_def_syntax(env, UD_BUILTIN_DEFINE, ud_builtin_define, alloc);
    env = ud_env_def_syntax(env, UD_BUILTIN_LAMBDA, ud_builtin_lambda, alloc);
    env = ud_env_def_syntax(env, UD_BUILTIN_BLOCK, ud_builtin_block, alloc);
    env = ud_env_def_syntax(env, UD_BUILTIN_QUOTE, ud_builtin_quote, alloc);
    env = ud_env_def_syntax(env, UD_BUILTIN_IF, ud_builtin_if, alloc);

    return env;
}
