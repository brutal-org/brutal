#pragma once

#include <brutal/base/std.h>

typedef enum
{
    REFCOUNT_REF,
    REFCOUNT_DEREF,
    REFCOUNT_0,
} refcount_result_t;

typedef struct
{
    atomic_int refs;
} refcount_t;

static inline void refcount_init(refcount_t *refcount)
{
    atomic_init(&refcount->refs, 1);
}

static inline refcount_result_t refcount_ref(refcount_t *refcount)
{
    atomic_fetch_add(&refcount->refs, 1);
    return REFCOUNT_REF;
}

static inline refcount_result_t refcount_deref(refcount_t *refcount)
{
    if (atomic_fetch_sub(&refcount->refs, 1) == 1)
    {
        return REFCOUNT_0;
    }
    else
    {
        return REFCOUNT_DEREF;
    }
}
