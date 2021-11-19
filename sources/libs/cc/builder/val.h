#pragma once

#include <cc/ast/val.h>

#define cval$(VAL)            \
    _Generic((VAL),           \
             signed int       \
             : cval_signed,   \
               signed long    \
             : cval_signed,   \
               signed char    \
             : cval_signed,   \
               signed short   \
             : cval_signed,   \
               unsigned int   \
             : cval_unsigned, \
               unsigned long  \
             : cval_unsigned, \
               float          \
             : cval_float)(VAL)

CVal cval_signed(CSigned v);

CVal cval_unsigned(CUnsigned v);

CVal cval_float(double v);

CVal cval_string(Str str);
