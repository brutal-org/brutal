#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>
#include <library/base/types.h>

#define result_t(error_t, ok_t) \
    struct                      \
    {                           \
        bool success;           \
        union                   \
        {                       \
            ok_t ok;            \
            error_t error;      \
        };                      \
    }

#define OK(T, value)     \
    (T)                  \
    {                    \
        .success = true, \
        .ok = (value),   \
    }

#define ERR(T, value)     \
    (T)                   \
    {                     \
        .success = false, \
        .error = (value)  \
    }

#define TRY(T, expr)                     \
    ({                                   \
        auto result = (expr);            \
        if (!result.success)             \
        {                                \
            return ERR(T, result.error); \
        }                                \
        result.ok;                       \
    })
