#pragma once

#include <brutal/alloc/base.h>
#include <brutal/io/buffer.h>

typedef struct
{
    size_t size;
    uint8_t data[];
} LinearBufferNode;

typedef struct
{
    Buffer buffer;
    size_t current;
} LinearBufferImpl;

#define LinearBuffer(T)                                                            \
    union                                                                          \
    {                                                                              \
        LinearBufferImpl _impl;                                                    \
                                                                                   \
        /* Don't access this, it's a meta variable for storing the element type */ \
        T *_T;                                                                     \
    }

void linear_buffer_init_impl(LinearBufferImpl *impl, Alloc *alloc);

void linear_buffer_deinit_impl(LinearBufferImpl *impl);

void linear_buffer_push_impl(LinearBufferImpl *impl, uint8_t const *data, size_t size);

void linear_buffer_attach_impl(LinearBufferImpl *impl, uint8_t const *data, size_t size);

void linear_buffer_clear_impl(LinearBufferImpl *impl);

#define linear_buffer_init(SELF, ALLOC) \
    linear_buffer_init_impl(impl_cast(SELF), (ALLOC))

#define linear_buffer_deinit(SELF) \
    linear_buffer_deinit_impl(impl_cast(SELF))

#define linear_buffer_push(SELF, VALUE) (                                             \
    {                                                                                 \
        auto tmp = (VALUE);                                                           \
        linear_buffer_push_impl(impl_cast(SELF), (uint8_t const *)&tmp, sizeof(tmp)); \
    })

#define linear_buffer_attach(SELF, VALUE) ({})

#define linear_buffer_clear(SELF) \
    linear_buffer_clear_impl(impl_cast(SELF))

#define linear_buffer_foreach(VAR, SELF)                                         \
    for (auto __it = (LinearBufferNode *)buffer_begin(&impl_cast(SELF)->buffer); \
         __it < (LinearBufferNode *)buffer_end(&impl_cast(SELF)->buffer);        \
         __it = (LinearBufferNode *)(((uint8_t *)__it) + __it->size))            \
        for (auto VAR = (typeof((SELF)->_T))&__it->data;                         \
             VAR;                                                                \
             VAR = nullptr)
