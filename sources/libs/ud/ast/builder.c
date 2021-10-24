#include <ud/ast/builder.h>

UdExpr ud_nil(void)
{
    return (UdExpr){.type = UD_NIL};
}

UdExpr ud_bool(bool value)
{
    return (UdExpr){
        .type = UD_BOOL,
        .bool_ = value ? UD_TRUE : UD_FALSE,
    };
}

UdExpr ud_atom(Str atom)
{
    return (UdExpr){
        .type = UD_ATOM,
        .atom_ = atom,
    };
}

UdExpr ud_num(int64_t num)
{
    return (UdExpr){
        .type = UD_NUM,
        .num_ = num,
    };
}

UdExpr ud_rune(Rune rune)
{
    return (UdExpr){
        .type = UD_RUNE,
        .rune_ = rune,
    };
}

UdExpr ud_str(Str str)
{
    return (UdExpr){
        .type = UD_STR,
        .str_ = str,
    };
}

UdExpr ud_pair(Alloc *alloc, UdExpr lhs, UdExpr rhs)
{
    return (UdExpr){
        .type = UD_PAIR,
        .pair_ = {
            .lhs = alloc_move(alloc, lhs),
            .rhs = alloc_move(alloc, rhs),
        },
    };
}

UdExpr ud_builtin(UdBuiltin builtin)
{
    return (UdExpr){
        .type = UD_BUILTIN,
        .builtin_ = builtin,
    };
}

UdExpr ud_syntax(UdSyntax syntax)
{
    return (UdExpr){
        .type = UD_SYNTAX,
        .syntax_ = syntax,
    };
}

UdExpr ud_lambda(Alloc *alloc, UdExpr env, UdExpr parms, UdExpr body)
{
    return (UdExpr){
        .type = UD_LAMBDA,
        .lambda_ = {
            .env = alloc_move(alloc, env),
            .parms = alloc_move(alloc, parms),
            .body = alloc_move(alloc, body),
        },
    };
}
