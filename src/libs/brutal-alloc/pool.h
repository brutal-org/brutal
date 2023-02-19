#pragma once

#include "base.h"

typedef void *PoolCtorFn(Alloc *alloc);
typedef void PoolDtorFn(Alloc *alloc, void *obj);

typedef struct
{
    void **pool;
    size_t used;
    size_t size;

    Alloc *alloc;
    PoolCtorFn *ctor;
    PoolDtorFn *dtor;
} Pool;

void pool_init(Pool *self, size_t size, PoolCtorFn *ctor, PoolDtorFn *dtor, Alloc *alloc);

void *pool_acquire(Pool *self);

void pool_release(Pool *self, void *obj);
