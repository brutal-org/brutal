#pragma once

#include <brutal/alloc/base.h>
#include <brutal/io/buf.h>

typedef struct
{
    size_t size;
    uint8_t data[];
} LinearBufNode;

typedef struct
{
    Buf buf;
    size_t current;
} LinearBufImpl;

#define LinearBuf(T)                                                               \
    union                                                                          \
    {                                                                              \
        LinearBufImpl _impl;                                                       \
                                                                                   \
        /* Don't access this, it's a meta variable for storing the element type */ \
        T *_T;                                                                     \
    }

void linear_buf_init_impl(LinearBufImpl *impl, Alloc *alloc);

void linear_buf_deinit_impl(LinearBufImpl *impl);

void linear_buf_push_impl(LinearBufImpl *impl, uint8_t const *data, size_t size);

void linear_buf_attach_impl(LinearBufImpl *impl, uint8_t const *data, size_t size);

void linear_buf_clear_impl(LinearBufImpl *impl);

#define linear_buf_init(SELF, ALLOC) \
    linear_buf_init_impl(impl$(SELF), (ALLOC))

#define linear_buf_deinit(SELF) \
    linear_buf_deinit_impl(impl$(SELF))

#define linear_buf_push(SELF, VALUE) (                                         \
    {                                                                          \
        AutoType tmp = (VALUE);                                                \
        linear_buf_push_impl(impl$(SELF), (uint8_t const *)&tmp, sizeof(tmp)); \
    })

#define linear_buf_attach(SELF, VALUE) ({})

#define linear_buf_clear(SELF) \
    linear_buf_clear_impl(impl$(SELF))

#define linear_buf_foreach(VAR, SELF)                                   \
    for (AutoType __it = (LinearBufNode *)buf_begin(&impl$(SELF)->buf); \
         __it < (LinearBufNode *)buf_end(&impl$(SELF)->buf);            \
         __it = (LinearBufNode *)(((uint8_t *)__it) + __it->size))      \
        for (AutoType VAR = (typeof((SELF)->_T))&__it->data;            \
             VAR;                                                       \
             VAR = nullptr)
