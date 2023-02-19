#pragma once

#include <brutal-base>

typedef enum
{
    REFCOUNT_REF,
    REFCOUNT_DEREF,
    REFCOUNT_0,
} RefCountResult;

typedef struct
{
    atomic_int refs;
} RefCount;

static inline void refcount_init(RefCount *refcount)
{
    atomic_init(&refcount->refs, 1);
}

static inline RefCountResult refcount_ref(RefCount *refcount)
{
    atomic_fetch_add(&refcount->refs, 1);
    return REFCOUNT_REF;
}

static inline RefCountResult refcount_deref(RefCount *refcount)
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
