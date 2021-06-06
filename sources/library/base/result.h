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

#define TRY(expr)             \
    ({                        \
        auto result = (expr); \
        if (!result.success)  \
        {                     \
            return result;    \
        }                     \
        result.ok;            \
    })

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
