#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>

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

#define alloc_decommit(self, ptr) \
    ((self)->decommit((self), ptr))

#define alloc_release(self, ptr) \
    ((self)->commit((self), ptr))
