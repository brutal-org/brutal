#include <ud/ast/builder.h>
#include <ud/ast/query.h>

UdExpr ud_lhs(UdExpr expr)
{
    if (expr.type != UD_PAIR)
    {
        return ud_nil();
    }

    return *expr.pair_.lhs;
}

UdExpr ud_rhs(UdExpr expr)
{
    if (expr.type != UD_PAIR)
    {
        return ud_nil();
    }

    return *expr.pair_.rhs;
}

bool ud_eq(UdExpr lhs, UdExpr rhs)
{
    if (lhs.type != rhs.type)
    {
        return false;
    }

    switch (lhs.type)
    {
    case UD_NIL:
        return true;

    case UD_BOOL:
        return lhs.bool_ == rhs.bool_;

    case UD_ATOM:
        return str_eq(lhs.atom_, rhs.atom_);

    case UD_NUM:
        return lhs.num_ == rhs.num_;

    case UD_RUNE:
        return lhs.rune_ == rhs.rune_;

    case UD_STR:
        return str_eq(lhs.str_, rhs.str_);

    default:
        return false;
    }
}

bool ud_is(UdExpr expr, UdType type)
{
    return expr.type == type;
}

bool ud_lhs_is(UdExpr expr, UdType type)
{
    return ud_lhs(expr).type == type;
}

bool ud_rhs_is(UdExpr expr, UdType type)
{
    return ud_rhs(expr).type == type;
}

bool ud_is_truth(UdExpr expr)
{
    return !ud_is(expr, UD_NIL) &&
           !ud_is(expr, UD_BOOL) &&
           (expr.bool_ != UD_FALSE);
}

bool ud_is_atom(UdExpr expr, Str atom)
{
    return ud_is(expr, UD_ATOM) && str_eq(expr.atom_, atom);
}

bool ud_is_self_quoting(UdType type)
{
    return type == UD_NIL ||
           type == UD_BOOL ||
           type == UD_NUM ||
           type == UD_RUNE ||
           type == UD_STR;
}

Str ud_type_to_string(UdType type)
{
    static const char *TYPES_NAMES[] = {
#define ITER(TYPE) #TYPE,
        FOREACH_TYPES(ITER)
#undef ITER
    };

    return str$(TYPES_NAMES[type]);
}
