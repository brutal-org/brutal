#pragma once

#define STDC_RESTRICT __restrict
#define STDC_NORETURN __attribute__((noreturn))

#ifdef __cplusplus
#    define STDC_BEGIN_HEADER \
        extern "C"            \
        {
#    define STDC_END_HEADER }
#else
#    define STDC_BEGIN_HEADER
#    define STDC_END_HEADER
#endif
