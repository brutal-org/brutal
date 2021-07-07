#pragma once

#include <brutal/alloc/base.h>

typedef struct
{
    size_t size;
    uint8_t data[];
} LinearBufferNodeImpl;

typedef struct
{
    LinearBufferNodeImpl *head;
    LinearBufferNodeImpl *tail;

    size_t allocated;
    struct alloc *alloc;
} LinearBufferImpl;

#define LinearBufferNode(T)         \
    union                           \
    {                               \
        struct                      \
        {                           \
            size_t size;            \
            T data;                 \
        };                          \
        LinearBufferNodeImpl _impl; \
    }

#define LinearBuffer(T)                 \
    union                               \
    {                                   \
        struct                          \
        {                               \
            LinearBufferNode(T) * head; \
            LinearBufferNode(T) * tail; \
        };                              \
        LinearBufferImpl _impl;         \
    }

void linear_buffer_init_impl(LinearBufferImpl *impl, struct alloc *alloc);

void linear_buffer_deinit_impl(LinearBufferImpl *impl);

void linear_buffer_push_impl(LinearBufferImpl *impl, void *data, size_t size);

void linear_buffer_clear_impl(LinearBufferImpl *impl);

#define linear_buffer_init(SELF, ALLOC) \
    linear_buffer_init_impl(&(SELF)->_impl, (ALLOC))

#define linear_buffer_deinit(SELF) \
    linear_buffer_deinit_impl(&(SELF)->_impl)

#define linear_buffer_push(SELF, VALUE) (                          \
    {                                                              \
        auto tmp = (VALUE);                                        \
        linear_buffer_push_impl(&(SELF)->_impl, &tmp, sizeof(tmp)) \
    })

#define linear_buffer_clear(SELF) \
    linear_buffer_clear_impl(&(SELF)->_impl)
