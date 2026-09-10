#pragma once

#include "std.h"

typedef struct
{
    void const *buf;
    size_t len;

    size_t size;
} SliceImpl;

#define slice_impl$(SLICE) \
    (SliceImpl) { .len = (SLICE).len, .size = sizeof(*(SLICE).buf), .buf = (SLICE).buf, }

#define Slice(T)      \
    struct            \
    {                 \
        T const *buf; \
        size_t len;   \
    }

typedef Slice(void) VoidSlice;

#define slice$(type, buffer, size) \
    (type) { .buf = buffer, .len = size }

#define slice_array$(type, buffer) \
    (type) { .buf = (buffer), .len = sizeof(buffer) / sizeof(*buffer), }

#define slice_begin$(SLICE) ((SLICE).buf)

#define slice_end$(SLICE) ((SLICE).buf + (SLICE).len)

#define slice_foreach$(VAR, SELF) \
    if ((SELF).len)               \
        for (typeof((SELF).buf) VAR = slice_begin$(SELF); VAR != slice_end$(SELF); VAR++)

#define slice_foreach_rev$(VAR, SELF) \
    if ((SELF).len)                   \
        for (typeof((SELF).buf) VAR = slice_end$(SELF) - 1; VAR >= slice_begin$(SELF); VAR--)
