#pragma once

#define COMPILER_GCC 0
#define COMPILER_CLANG 0

#if defined(__GNUC__)
#    if defined(__clang__)
#        undef COMPILER_CLANG
#        define COMPILER_CLANG 1
#    else
#        undef COMPILER_GCC
#        define COMPILER_GCC 1
#    endif
#elif
#    error "Unsupported compiler!"
#endif
