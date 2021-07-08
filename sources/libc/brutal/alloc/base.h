#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>
#include <brutal/mem.h>

typedef struct alloc Alloc;

void alloc_no_op(Alloc *self, void *ptr);

typedef void *alloc_acquire_t(Alloc *self, size_t size);
typedef void alloc_commit_t(Alloc *self, void *ptr);
typedef void alloc_decommit_t(Alloc *self, void *ptr);
typedef void alloc_release_t(Alloc *self, void *ptr);

struct alloc
{
    alloc_acquire_t *acquire;
    alloc_commit_t *commit;
    alloc_decommit_t *decommit;
    alloc_release_t *release;
};

#define alloc_acquire(SELF, SIZE) \
    ((SELF)->acquire((SELF), SIZE))

#define alloc_commit(SELF, PTR) \
    ((SELF)->commit((SELF), PTR))

#define alloc_make(SELF, T) (                    \
    {                                            \
        T *ptr = alloc_acquire(SELF, sizeof(T)); \
        alloc_commit(SELF, ptr);                 \
        mem_set(ptr, 0, sizeof(T));              \
        ptr;                                     \
    })

#define alloc_make_array(SELF, T, COUNT) \
    ((T *)alloc_calloc(SELF, sizeof(T), COUNT))

#define alloc_decommit(SELF, PTR) \
    ((SELF)->decommit((SELF), PTR))

#define alloc_release(SELF, PTR) \
    ((SELF)->commit((SELF), PTR))

#define alloc_malloc(SELF, SIZE) (             \
    {                                          \
        void *ptr = alloc_acquire(SELF, SIZE); \
        alloc_commit(SELF, ptr);               \
        ptr;                                   \
    })

#define alloc_free(SELF, PTR) (    \
    {                              \
        alloc_decommit(SELF, PTR); \
        alloc_release(SELF, PTR);  \
    })

#define alloc_calloc(SELF, SIZE, COUNT) (              \
    {                                                  \
        void *ptr = alloc_acquire(SELF, SIZE * COUNT); \
        alloc_commit(SELF, ptr);                       \
        mem_set(ptr, 0, SIZE *COUNT);                  \
        ptr;                                           \
    })
