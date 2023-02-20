#pragma once

// If you define this macros your are a bad programmer, or writing a c library.
#ifndef _BRUTAL_ALLOW_BANNED

#    define BANNED(FUNCTION) sorry_##FUNCTION##_is_a_banned_function

#    undef strcpy
#    undef strcat
#    undef strncpy
#    undef strncat
#    undef printf
#    undef sprintf
#    undef vsprintf

#    define strcpy BANNED(strcpy)
#    define strcat BANNED(strcat)
#    define strncpy BANNED(strncpy)
#    define strncat BANNED(strncat)
#    define printf BANNED(printf)
#    define sprintf BANNED(sprintf)
#    define vsprintf BANNED(vsprintf)

#endif
