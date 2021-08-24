#pragma once

#include <brutal/base/keywords.h>
#include <brutal/base/std.h>

#define Result(ERROR, OK) \
    struct                \
    {                     \
        bool succ;        \
                          \
        union             \
        {                 \
            ERROR err;    \
            OK ok;        \
        };                \
    }

#define OK(T, VALUE)   \
    (T)                \
    {                  \
        .succ = true,  \
        .ok = (VALUE), \
    }

#define ERR(T, ERROR)  \
    (T)                \
    {                  \
        .succ = false, \
        .err = (ERROR) \
    }

#define TRY(T, EXPR) (                 \
    {                                  \
        auto __expr = (EXPR);          \
                                       \
        if (!__expr.succ)              \
        {                              \
            return ERR(T, __expr.err); \
        }                              \
                                       \
        __expr.ok;                     \
    })

#define UNWRAP_OR_MESSAGE(EXPR, MESSAGE) ( \
    {                                      \
        auto __expr = (EXPR);              \
                                           \
        if (!__expr.succ)                  \
        {                                  \
            panic$(MESSAGE);               \
        }                                  \
                                           \
        __expr.ok;                         \
    })

#define UNWRAP_OR(EXPR, VALUE) (    \
    {                               \
        auto __expr = (EXPR);       \
        typeof(__expr.ok) __result; \
                                    \
        if (__expr.succ)            \
        {                           \
            __result = __expr.ok;   \
        }                           \
        else                        \
        {                           \
            __result = (VALUE);     \
        }                           \
                                    \
        __result;                   \
    })

#define UNWRAP(EXPR) \
    UNWRAP_OR_MESSAGE(EXPR, "UNWRAP(" #EXPR ") failled")
