#include <brutal-debug>

#include "pool.h"

void pool_init(Pool *self, size_t size, PoolCtorFn *ctor, PoolDtorFn *dtor, Alloc *alloc)
{
    self->pool = alloc_make_array(alloc, void *, size);
    self->size = size;
    self->used = 0;
    self->ctor = ctor;
    self->dtor = dtor;
    self->alloc = alloc;
}

void *pool_acquire(Pool *self)
{
    if (self->used)
    {
        return self->pool[--self->used];
    }

    return self->ctor(self->alloc);
}

void pool_release(Pool *self, void *obj)
{
    if (self->used == self->size)
    {
        self->dtor(self->alloc, obj);
        return;
    }

    self->pool[self->used++] = obj;
}
