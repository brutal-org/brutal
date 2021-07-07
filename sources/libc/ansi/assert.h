#pragma once

/* --- 7.2 Diagnostics <assert.h> ----------------------------------------------- */

void assert_failled_impl();

#ifdef NDEBUG
#    define assert(expr) (void)(expr)
#else
#    define assert(expr)           \
        if (!expr)                 \
        {                          \
            assert_failled_impl(); \
        }
#endif

#define static_assert _Static_assert
