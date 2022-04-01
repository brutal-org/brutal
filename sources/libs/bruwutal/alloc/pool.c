#include <bruwutal/alloc/pool.h>
#include <bruwutal/debug/locked.h>

void pool_init(Pool *self, PoolCtorFn *ctor, PoolDtorFn *dtor, Alloc *alloc)
{
    vec_init(self, alloc);
    self->ctor = ctor;
    self->dtor = dtor;
}

void *pool_acquire(Pool *self)
{
    if (vec_len(self))
    {
        return vec_pop(self);
    }

    return self->ctor(self->_impl.alloc);
}

void pool_release(Pool *self, void *obj)
{
    vec_push(self, obj);
}
