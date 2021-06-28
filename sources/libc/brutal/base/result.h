#pragma once

#include <brutal/base/keywords.h>
#include <brutal/base/std.h>
#include <brutal/base/types.h>

#define result_t(error_t, ok_t) \
    struct                      \
    {                           \
        bool success;           \
        union                   \
        {                       \
            ok_t _ok;           \
            error_t _error;     \
        };                      \
    }

#define OK(T, value)     \
    (T)                  \
    {                    \
        .success = true, \
        ._ok = (value),  \
    }

#define ERR(T, value)     \
    (T)                   \
    {                     \
        .success = false, \
        ._error = (value) \
    }

#define TRY(T, expr)                          \
    (                                         \
        {                                     \
            auto result = (expr);             \
            if (!result.success)              \
            {                                 \
                return ERR(T, result._error); \
            }                                 \
            result._ok;                       \
        })

#define UNWRAP(expr) (                          \
    {                                           \
        auto expr_value = (expr);               \
                                                \
        if (!expr_value.success)                \
        {                                       \
            panic("UNWRAP(" #expr ") failled"); \
        }                                       \
                                                \
        expr_value._ok;                         \
    })
