#pragma once

#include <stdc-base/prelude.h>

STDC_BEGIN_HEADER

/* --- 7.2 Diagnostics ------------------------------------------------------ */

void assert_failed_impl(void);

#ifdef NDEBUG
#    define assert(expr) (void)(expr)
#else
#    define assert(expr)          \
        if (!expr)                \
        {                         \
            assert_failed_impl(); \
        }
#endif

#define static_assert _Static_assert

STDC_END_HEADER
