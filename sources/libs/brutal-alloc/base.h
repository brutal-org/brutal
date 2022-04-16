#pragma once

#include <brutal-base>
#include <brutal-mem>

typedef struct _Alloc Alloc;

void alloc_no_op(Alloc *self, void *ptr);

typedef void *AllocAcquire(Alloc *self, size_t size);

typedef void *AllocResize(Alloc *self, void *ptr, size_t new_size);

typedef void AllocRelease(Alloc *self, void *ptr);

typedef enum
{
    NODE_SMALL = KiB(4),
    NODE_DEFAULT = KiB(16),
    NODE_LARGE = KiB(128),
} NodeSize;

struct _Alloc
{
    AllocAcquire *acquire;
    AllocResize *resize;
    AllocRelease *release;
};

#define alloc_acquire(SELF, SIZE) \
    ((SELF)->acquire((SELF), (SIZE)))

#define alloc_resize(SELF, PTR, NEW_SIZE) \
    ((SELF)->resize((SELF), (PTR), (NEW_SIZE)))

#define alloc_release(SELF, PTR) \
    ((SELF)->release((SELF), (PTR)))

#define alloc_make(SELF, T) (                           \
    {                                                   \
        T *__ptr = (T *)alloc_acquire(SELF, sizeof(T)); \
        *__ptr = (T){};                                 \
        __ptr;                                          \
    })

// Move an object to the heap
#define alloc_move(SELF, VALUE) (                                                 \
    {                                                                             \
        typeof(VALUE) *ptr = (typeof(VALUE) *)alloc_acquire(SELF, sizeof(VALUE)); \
        mem_cpy(ptr, &(VALUE), sizeof(VALUE));                                    \
        ptr;                                                                      \
    })

#define alloc_make_array(SELF, T, COUNT) \
    ((T *)alloc_calloc(SELF, sizeof(T), COUNT))

/* --- Vanilla Allocation Functions ----------------------------------------- */

void *alloc_malloc(Alloc *self, size_t size);

void alloc_free(Alloc *self, void *ptr);

void *alloc_calloc(Alloc *self, size_t size, size_t count);
