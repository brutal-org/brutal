#pragma once

#include <cc/ast/type.h>
#include <cc/ast/val.h>

Str cval_to_str(CVal type);

CType cval_type(CVal value, Alloc *alloc);

/* --- Builders ------------------------------------------------------------- */

CVal cval_error(void);

CVal cval_char(CSigned val);

CVal cval_short(CSigned val);

CVal cval_int(CSigned val);

CVal cval_long(CSigned val);

CVal cval_long_long(CSigned val);

CVal cval_uchar(CUnsigned val);

CVal cval_ushort(CUnsigned val);

CVal cval_uint(CUnsigned val);

CVal cval_ulong(CUnsigned val);

CVal cval_ulonglong(CUnsigned val);

CVal cval_float(double val);

CVal cval_double(double val);

CVal cval_str(Str str);
