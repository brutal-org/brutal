#pragma once

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
