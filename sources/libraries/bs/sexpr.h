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
#define ITER(TYPE) BS_VAL_##TYPE,
    FOREACH_TYPES(ITER)
#undef ITER
} BsType;

static inline Str bs_type_to_string(BsType type)
{
    static const char *TYPES_NAMES[] = {
#define ITER(TYPE) #TYPE,
        FOREACH_TYPES(ITER)
#undef ITER
    };

    return str_cast(TYPES_NAMES[type]);
}

#undef FOREACH_TYPES

typedef struct bs_expr BsExpr;

typedef struct
{
} BsNil;

typedef enum
{
    BS_FALSE,
    BS_TRUE,
} BsBool;

typedef Str BsAtom;

typedef int64_t BsNum;

typedef Rune BsRune;

typedef Str BsStr;

typedef struct
{
    BsExpr *car;
    BsExpr *cdr;
} BsPair;

typedef BsExpr (*BsBuiltin)(BsExpr args, BsExpr *env, Alloc *alloc);
typedef BsBuiltin BsSyntax;

typedef struct
{
    BsExpr *env;
    BsExpr *parms;
    BsExpr *body;
} BsLambda;

struct bs_expr
{
    BsType type;

    union
    {
        BsNil nil_;
        BsBool bool_;
        BsAtom atom_;
        BsNum num_;
        BsRune rune_;
        BsStr str_;
        BsPair pair_;
        BsBuiltin builtin_;
        BsSyntax syntax_;
        BsLambda lambda_;
    };
};

static inline BsExpr bs_nil(void)
{
    return (BsExpr){.type = BS_VAL_NIL};
}

static inline BsExpr bs_bool(bool value)
{
    return (BsExpr){
        .type = BS_VAL_BOOL,
        .bool_ = value ? BS_TRUE : BS_FALSE,
    };
}

static inline BsExpr bs_atom(Str atom)
{
    return (BsExpr){
        .type = BS_VAL_ATOM,
        .atom_ = atom,
    };
}

static inline BsExpr bs_num(int64_t num)
{
    return (BsExpr){
        .type = BS_VAL_NUM,
        .num_ = num,
    };
}

static inline BsExpr bs_rune(Rune rune)
{
    return (BsExpr){
        .type = BS_VAL_RUNE,
        .rune_ = rune,
    };
}

static inline BsExpr bs_str(Str str)
{
    return (BsExpr){
        .type = BS_VAL_STR,
        .str_ = str,
    };
}

static inline BsExpr bs_pair(Alloc *alloc, BsExpr car, BsExpr cdr)
{
    return (BsExpr){
        .type = BS_VAL_PAIR,
        .pair_ = {
            .car = alloc_move(alloc, car),
            .cdr = alloc_move(alloc, cdr),
        },
    };
}

static inline BsExpr bs_builtin(BsBuiltin builtin)
{
    return (BsExpr){
        .type = BS_VAL_BUILTIN,
        .builtin_ = builtin,
    };
}

static inline BsExpr bs_syntax(BsSyntax syntax)
{
    return (BsExpr){
        .type = BS_VAL_SYNTAX,
        .syntax_ = syntax,
    };
}

static inline BsExpr bs_lambda(Alloc *alloc, BsExpr env, BsExpr parms, BsExpr body)
{
    return (BsExpr){
        .type = BS_VAL_LAMBDA,
        .lambda_ = {
            .env = alloc_move(alloc, env),
            .parms = alloc_move(alloc, parms),
            .body = alloc_move(alloc, body),
        },
    };
}

static inline BsExpr bs_car(BsExpr expr)
{
    if (expr.type != BS_VAL_PAIR)
    {
        return bs_nil();
    }

    return *expr.pair_.car;
}

static inline BsExpr bs_cdr(BsExpr expr)
{
    if (expr.type != BS_VAL_PAIR)
    {
        return bs_nil();
    }

    return *expr.pair_.cdr;
}

#define bs_caar(obj) bs_car(bs_car(obj))
#define bs_cadr(obj) bs_car(bs_cdr(obj))
#define bs_cdar(obj) bs_cdr(bs_car(obj))
#define bs_cddr(obj) bs_cdr(bs_cdr(obj))

#define bs_caaar(obj) bs_car(bs_car(bs_car(obj)))
#define bs_caadr(obj) bs_car(bs_car(bs_cdr(obj)))
#define bs_cadar(obj) bs_car(bs_cdr(bs_car(obj)))
#define bs_caddr(obj) bs_car(bs_cdr(bs_cdr(obj)))
#define bs_cdaar(obj) bs_cdr(bs_car(bs_car(obj)))
#define bs_cdadr(obj) bs_cdr(bs_car(bs_cdr(obj)))
#define bs_cddar(obj) bs_cdr(bs_cdr(bs_car(obj)))
#define bs_cdddr(obj) bs_cdr(bs_cdr(bs_cdr(obj)))

#define bs_caaaar(obj) bs_car(bs_car(bs_car(bs_car(obj))))
#define bs_caaadr(obj) bs_car(bs_car(bs_car(bs_cdr(obj))))
#define bs_caadar(obj) bs_car(bs_car(bs_cdr(bs_car(obj))))
#define bs_caaddr(obj) bs_car(bs_car(bs_cdr(bs_cdr(obj))))
#define bs_cadaar(obj) bs_car(bs_cdr(bs_car(bs_car(obj))))
#define bs_cadadr(obj) bs_car(bs_cdr(bs_car(bs_cdr(obj))))
#define bs_caddar(obj) bs_car(bs_cdr(bs_cdr(bs_car(obj))))
#define bs_cadddr(obj) bs_car(bs_cdr(bs_cdr(bs_cdr(obj))))
#define bs_cdaaar(obj) bs_cdr(bs_car(bs_car(bs_car(obj))))
#define bs_cdaadr(obj) bs_cdr(bs_car(bs_car(bs_cdr(obj))))
#define bs_cdadar(obj) bs_cdr(bs_car(bs_cdr(bs_car(obj))))
#define bs_cdaddr(obj) bs_cdr(bs_car(bs_cdr(bs_cdr(obj))))
#define bs_cddaar(obj) bs_cdr(bs_cdr(bs_car(bs_car(obj))))
#define bs_cddadr(obj) bs_cdr(bs_cdr(bs_car(bs_cdr(obj))))
#define bs_cdddar(obj) bs_cdr(bs_cdr(bs_cdr(bs_car(obj))))
#define bs_cddddr(obj) bs_cdr(bs_cdr(bs_cdr(bs_cdr(obj))))

static inline bool bs_eq(BsExpr lhs, BsExpr rhs)
{
    if (lhs.type != rhs.type)
    {
        return false;
    }

    switch (lhs.type)
    {
    case BS_VAL_NIL:
        return true;

    case BS_VAL_BOOL:
        return lhs.bool_ == rhs.bool_;

    case BS_VAL_ATOM:
        return str_eq(lhs.atom_, rhs.atom_);

    case BS_VAL_NUM:
        return lhs.num_ == rhs.num_;

    case BS_VAL_RUNE:
        return lhs.rune_ == rhs.rune_;

    case BS_VAL_STR:
        return str_eq(lhs.str_, rhs.str_);

    default:
        return false;
    }
}

static inline bool bs_is(BsExpr expr, BsType type)
{
    return expr.type == type;
}

static inline bool bs_car_is(BsExpr expr, BsType type)
{
    return bs_car(expr).type == type;
}

static inline bool bs_cdr_is(BsExpr expr, BsType type)
{
    return bs_cdr(expr).type == type;
}

static inline bool bs_is_truth(BsExpr expr)
{
    return !bs_is(expr, BS_VAL_NIL) &&
           !bs_is(expr, BS_VAL_BOOL) &&
           (expr.bool_ != BS_FALSE);
}

static inline bool bs_is_self_quoting(BsType type)
{
    return type == BS_VAL_NIL ||
           type == BS_VAL_BOOL ||
           type == BS_VAL_NUM ||
           type == BS_VAL_RUNE ||
           type == BS_VAL_STR;
}
