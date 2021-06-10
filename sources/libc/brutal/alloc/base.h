#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>
#include <brutal/mem.h>

struct alloc;

void alloc_no_op(struct alloc *self, void *ptr);

typedef void *alloc_acquire_t(struct alloc *self, size_t size);
typedef void alloc_commit_t(struct alloc *self, void *ptr);
typedef void alloc_decommit_t(struct alloc *self, void *ptr);
typedef void alloc_release_t(struct alloc *self, void *ptr);

struct alloc
{
    alloc_acquire_t *acquire;
    alloc_commit_t *commit;
    alloc_decommit_t *decommit;
    alloc_release_t *release;
};

#define alloc_acquire(self, size) \
    ((self)->acquire((self), size))

#define alloc_commit(self, ptr) \
    ((self)->commit((self), ptr))

#define alloc_make(self, T) (                    \
    {                                            \
        T *ptr = alloc_acquire(self, sizeof(T)); \
        alloc_commit(self, ptr);                 \
        mem_set(ptr, 0, sizeof(T));              \
        ptr;                                     \
    })

#define alloc_decommit(self, ptr) \
    ((self)->decommit((self), ptr))

#define alloc_release(self, ptr) \
    ((self)->commit((self), ptr))
