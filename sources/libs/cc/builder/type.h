#pragma once

#include <cc/ast/type.h>

Str ctype_to_str(CType type);

CType ctype_to_unsigned(CType type);

/* --- Builders ------------------------------------------------------------- */

CType ctype_error(void);

CType ctype_tail(void);

CType ctype_void(void);

CType ctype_auto(void);

CType ctype_bool(void);

CType ctype_ptr(CType subtype, Alloc *alloc);

CType ctype_parent(CType subtype, Alloc *alloc);

CType ctype_array(CType subtype, int size, Alloc *alloc);

CType ctype_char(void);

CType ctype_short(void);

CType ctype_int(void);

CType ctype_long(void);

CType ctype_longlong(void);

CType ctype_uchar(void);

CType ctype_ushort(void);

CType ctype_uint(void);

CType ctype_ulong(void);

CType ctype_ulonglong(void);

CType ctype_float(void);

CType ctype_double(void);

CType ctype_struct(Alloc *alloc);

CType ctype_union(Alloc *alloc);

CType ctype_enum(Alloc *alloc);

void ctype_constant(CType *self, Str name, CVal val);

CType ctype_func(CType ret, Alloc *alloc);

CType ctype_ident(Str name);

#define ctype_ident_ptr(IDENT, ALLOC) \
    ctype_ptr(ctype_ident(IDENT), ALLOC)

CType ctype_attr(CType type, CTypeAttr attr);

CType ctype_named(CType type, Str name);

void ctype_member(CType *self, Str name, CType type);

void ctype_append(CType *type, CType new_tail);
