#pragma once

#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/text.h>

#define FOREACH_TYPES(TYPE) \
    TYPE(NIL)               \
    TYPE(BOOL)              \
    TYPE(ATOM)              \
    TYPE(NUM)               \
    TYPE(RUNE)              \
    TYPE(STR)               \
    TYPE(PAIR)              \
    TYPE(BUILTIN)           \
    TYPE(SYNTAX)            \
    TYPE(LAMBDA)

typedef enum
{
#define ITER(TYPE) UD_##TYPE,
    FOREACH_TYPES(ITER)
#undef ITER
} UdType;

static inline Str ud_type_to_string(UdType type)
{
    static const char *TYPES_NAMES[] = {
#define ITER(TYPE) #TYPE,
        FOREACH_TYPES(ITER)
#undef ITER
    };

    return str$(TYPES_NAMES[type]);
}

#undef FOREACH_TYPES

typedef struct ud_expr UdExpr;

typedef struct
{
    int _dummy;
} UdNil;

typedef enum
{
    UD_FALSE,
    UD_TRUE,
} UdBool;

typedef Str UdAtom;

typedef int64_t UdNum;

typedef Rune UdRune;

typedef Str UdStr;

typedef struct
{
    UdExpr *lhs;
    UdExpr *rhs;
} UdPair;

typedef UdExpr (*UdBuiltin)(UdExpr args, UdExpr *env, Alloc *alloc);
typedef UdBuiltin UdSyntax;

typedef struct
{
    UdExpr *env;
    UdExpr *parms;
    UdExpr *body;
} UdLambda;

struct ud_expr
{
    UdType type;

    union
    {
        UdNil nil_;
        UdBool bool_;
        UdAtom atom_;
        UdNum num_;
        UdRune rune_;
        UdStr str_;
        UdPair pair_;
        UdBuiltin builtin_;
        UdSyntax syntax_;
        UdLambda lambda_;
    };
};

static inline UdExpr ud_nil(void)
{
    return (UdExpr){.type = UD_NIL};
}

static inline UdExpr ud_bool(bool value)
{
    return (UdExpr){
        .type = UD_BOOL,
        .bool_ = value ? UD_TRUE : UD_FALSE,
    };
}

static inline UdExpr ud_atom(Str atom)
{
    return (UdExpr){
        .type = UD_ATOM,
        .atom_ = atom,
    };
}

static inline UdExpr ud_num(int64_t num)
{
    return (UdExpr){
        .type = UD_NUM,
        .num_ = num,
    };
}

static inline UdExpr ud_rune(Rune rune)
{
    return (UdExpr){
        .type = UD_RUNE,
        .rune_ = rune,
    };
}

static inline UdExpr ud_str(Str str)
{
    return (UdExpr){
        .type = UD_STR,
        .str_ = str,
    };
}

static inline UdExpr ud_pair(Alloc *alloc, UdExpr lhs, UdExpr rhs)
{
    return (UdExpr){
        .type = UD_PAIR,
        .pair_ = {
            .lhs = alloc_move(alloc, lhs),
            .rhs = alloc_move(alloc, rhs),
        },
    };
}

static inline UdExpr ud_builtin(UdBuiltin builtin)
{
    return (UdExpr){
        .type = UD_BUILTIN,
        .builtin_ = builtin,
    };
}

static inline UdExpr ud_syntax(UdSyntax syntax)
{
    return (UdExpr){
        .type = UD_SYNTAX,
        .syntax_ = syntax,
    };
}

static inline UdExpr ud_lambda(Alloc *alloc, UdExpr env, UdExpr parms, UdExpr body)
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

static inline UdExpr ud_lhs(UdExpr expr)
{
    if (expr.type != UD_PAIR)
    {
        return ud_nil();
    }

    return *expr.pair_.lhs;
}

static inline UdExpr ud_rhs(UdExpr expr)
{
    if (expr.type != UD_PAIR)
    {
        return ud_nil();
    }

    return *expr.pair_.rhs;
}

#define ud_llhs(obj) ud_lhs(ud_lhs(obj))
#define ud_lrhs(obj) ud_lhs(ud_rhs(obj))
#define ud_rlhs(obj) ud_rhs(ud_lhs(obj))
#define ud_rrhs(obj) ud_rhs(ud_rhs(obj))

static inline bool ud_eq(UdExpr lhs, UdExpr rhs)
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

static inline bool ud_is(UdExpr expr, UdType type)
{
    return expr.type == type;
}

static inline bool ud_lhs_is(UdExpr expr, UdType type)
{
    return ud_lhs(expr).type == type;
}

static inline bool ud_rhs_is(UdExpr expr, UdType type)
{
    return ud_rhs(expr).type == type;
}

static inline bool ud_is_truth(UdExpr expr)
{
    return !ud_is(expr, UD_NIL) &&
           !ud_is(expr, UD_BOOL) &&
           (expr.bool_ != UD_FALSE);
}

static inline bool ud_is_self_quoting(UdType type)
{
    return type == UD_NIL ||
           type == UD_BOOL ||
           type == UD_NUM ||
           type == UD_RUNE ||
           type == UD_STR;
}
