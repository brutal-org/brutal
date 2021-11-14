#pragma once

#include <brutal/base/std.h>

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
