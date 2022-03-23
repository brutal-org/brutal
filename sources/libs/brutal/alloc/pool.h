#pragma once

#include <brutal/ds.h>

typedef void *PoolCtorFn(Alloc *alloc);
typedef void PoolDtorFn(Alloc *alloc, void *obj);

typedef struct
{
    Vec(void *);
    PoolCtorFn *ctor;
    PoolDtorFn *dtor;
} Pool;

void pool_init(Pool *self, PoolCtorFn *ctor, PoolDtorFn *dtor, Alloc *alloc);

void *pool_acquire(Pool *self);

void pool_release(Pool *self, void *obj);
