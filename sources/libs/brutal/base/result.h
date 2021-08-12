#pragma once

#include <brutal/base/keywords.h>
#include <brutal/base/std.h>
#include <brutal/base/types.h>

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

#define TRY(T, EXPR) (                   \
    {                                    \
        auto __result = (EXPR);          \
                                         \
        if (!__result.succ)              \
        {                                \
            return ERR(T, __result.err); \
        }                                \
                                         \
        __result.ok;                     \
    })

#define UNWRAP_OR_MESSAGE(EXPR, MESSAGE) ( \
    {                                      \
        auto __result = (EXPR);            \
                                           \
        if (!__result.succ)                \
        {                                  \
            panic(MESSAGE);                \
        }                                  \
                                           \
        __result.ok;                       \
    })

#define UNWRAP(EXPR) \
    UNWRAP_OR_MESSAGE(EXPR, "UNWRAP(" #EXPR ") failled")
