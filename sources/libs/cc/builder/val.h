#pragma once

#include <cc/ast/val.h>

CVal cval_signed(CSigned v);

CVal cval_unsigned(CUnsigned v);

CVal cval_float(double v);

CVal cval_string(Str str, Alloc *alloc);
