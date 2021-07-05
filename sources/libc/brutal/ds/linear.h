#pragma once

#include <brutal/alloc/base.h>

typedef struct LinearBufferImpl
{
    void *head;
    void *tail;
    size_t allocated;
    struct alloc *alloc;
};

#define LinearBuffer(T)        \
    union                      \
    {                          \
        struct                 \
        {                      \
            T *head;           \
            T *tail;           \
            size_t allocated;  \
        };                     \
        LinearBufferImpl impl; \
    }
