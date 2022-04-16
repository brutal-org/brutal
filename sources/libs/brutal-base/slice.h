#pragma once

#include "std.h"

typedef struct
{
    size_t len;
    size_t size;
    void const *buf;
} SliceImpl;

#define slice_impl$(SLICE) \
    (SliceImpl) { .len = (SLICE).len, .size = sizeof(*(SLICE).buf), .buf = (SLICE).buf, }

#define InlineBuf(T) \
    struct           \
    {                \
        size_t len;  \
        T buf[];     \
    }

#define Slice(T)      \
    struct            \
    {                 \
        size_t len;   \
        T const *buf; \
    }

typedef Slice(void) VoidSlice;

#define slice$(type, buffer, size) \
    (type) { .len = size, .buf = buffer }

#define slice_array$(type, buffer) \
    (type) { .len = sizeof(buffer) / sizeof(*buffer), .buf = &(buffer) }
