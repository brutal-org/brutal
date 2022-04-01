#pragma once

#ifndef I_M_A_BAD_PROGRAMMER_OR_I_M_WRITING_A_C_LIBRARY

#    define BANNED(FUNCTION) sorry_##FUNCTION##_is_a_banned_function

#    undef strcpy
#    undef strcat
#    undef strncpy
#    undef strncat
#    undef sprintf
#    undef vsprintf

#    define strcpy BANNED(strcpy)
#    define strcat BANNED(strcat)
#    define strncpy BANNED(strncpy)
#    define strncat BANNED(strncat)
#    define sprintf BANNED(sprintf)
#    define vsprintf BANNED(vsprintf)

#endif
