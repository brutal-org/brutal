#pragma once

#include <cc/ast/type.h>

CType ctype_error(void);

CType ctype_tail(void);

CType ctype_void(void);

CType ctype_auto(void);

CType ctype_bool(void);

CType ctype_ptr(CType subtype, Alloc *alloc);

CType ctype_parent(CType subtype, Alloc *alloc);

CType ctype_array(CType subtype, int size, Alloc *alloc);

CType ctype_signed(int precision);

CType ctype_unsigned(int precision);

CType ctype_float(int precision);

CType ctype_struct(Alloc *alloc);

CType ctype_union(Alloc *alloc);

void ctype_member(CType *self, Str name, CType type, Alloc *alloc);

CType ctype_enum(Alloc *alloc);

void ctype_constant(CType *self, Str name, CVal val, Alloc *alloc);

CType ctype_func(CType ret, Alloc *alloc);

CType ctype_name(Str name, Alloc *alloc);

CType ctype_attr(CType type, CTypeAttr attr);

CType ctype_named(CType type, Str name, Alloc *alloc);

CType ctype_append(CType type, CType new_tail, Alloc *alloc);
