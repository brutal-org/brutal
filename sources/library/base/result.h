#pragma once

#include <library/base/macros.h>

#define result_t(ok_t) \
    struct             \
    {                  \
        ok_t ok;       \
        error_t error; \
    }

#define TRY(expr)                                  \
    ({                                             \
        auto result = (expr);                      \
        if (result.error.kind != ERR_KIND_SUCCESS) \
        {                                          \
            return result;                         \
        }                                          \
        result.ok;                                 \
    })

#define OK(value)     \
    {                 \
        .ok = (value) \
    }

#define ERR(value)       \
    {                    \
        .error = (value) \
    }
