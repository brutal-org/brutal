#pragma once

#include <base-macros.h>
#include <base-std.h>

struct alloc;

void alloc_no_op(struct alloc *self, void *ptr)
{
    UNUSED(self);
    UNUSED(ptr);
}

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

/* --- Alloc Heap ----------------------------------------------------------- */

struct alloc_heap
{
    struct alloc base;
    struct alloc_major *root; // The root memory block acquired from the system.
    struct alloc_major *best; // The major with the most free memory.
};

void alloc_heap_init(struct alloc_heap *alloc);

/* --- Global Alloc --------------------------------------------------------- */

struct alloc *alloc_global();
