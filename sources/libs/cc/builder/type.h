#pragma once

#include <cc/ast/type.h>

CType ctype_void(Str name, Alloc *alloc);

CType ctype_auto(Str name, Alloc *alloc);

CType ctype_bool(Str name, Alloc *alloc);

CType ctype_ptr(CType subtype, Alloc *alloc);

CType ctype_array(Str name, CType subtype, int size, Alloc *alloc);

CType ctype_signed(Str name, int precision, Alloc *alloc);

CType ctype_unsigned(Str name, int precision, Alloc *alloc);

CType ctype_float(Str name, int precision, Alloc *alloc);

CType ctype_struct(Str name, Alloc *alloc);

CType ctype_union(Str name, Alloc *alloc);

void ctype_member(CType *self, Str name, CType type, Alloc *alloc);

CType ctype_enum(Str name, Alloc *alloc);

void ctype_constant(CType *self, Str name, CVal val, Alloc *alloc);

CType ctype_func(CType ret, Str name, Alloc *alloc);

CType ctype_name(Str name, Alloc *alloc);

CType ctype_attr(CType type, CTypeAttr attr);
