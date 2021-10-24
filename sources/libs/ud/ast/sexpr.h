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
