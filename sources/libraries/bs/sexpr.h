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

typedef BsExpr (*BsBuiltin)(BsExpr args);

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

static inline BsExpr br_lambda(Alloc *alloc, BsExpr env, BsExpr parms, BsExpr body)
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
