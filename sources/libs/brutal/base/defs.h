#pragma once

/* --- Compiler ------------------------------------------------------------- */

#define COMPILER_GCC 0
#define COMPILER_CLANG 0

#if (defined(__GNUC__) || defined(_MSC_VER)) && defined(__clang__)
#    undef COMPILER_CLANG
#    define COMPILER_CLANG 1
#elif defined(__GNUC__)
#    undef COMPILER_GCC
#    define COMPILER_GCC 1
#else
#    error "Unsupported compiler!"
#endif

/* --- System --------------------------------------------------------------- */

#if defined(__brutal__)
#    define SYSTEM_BRUTAL
#    if defined(__kernel__)
#        define SYSTEM_KERNEL
#    endif
#elif defined(__efi__)
#    define SYSTEM_EFI
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    define SYSTEM_WIN
#elif defined(__APPLE__)
#    define SYSTEM_DARWIN
#    define SYSTEM_POSIX
#    define SYSTEM_SDL
#elif defined(__linux__)
#    define SYSTEM_LINUX
#    define SYSTEM_POSIX
#    define SYSTEM_SDL
#elif defined(__unix__) // all unices not caught above
#    define SYSTEM_POSIX
#elif defined(_POSIX_VERSION)
#    define SYSTEM_POSIX
#else
#    error "Unknown system"
#endif
