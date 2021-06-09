#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>

struct lock
{
    int locked;
};

void lock_acquire(struct lock *lock);

void lock_release(struct lock *lock);

static inline void lock_retainer_release(struct lock **lock)
{
    lock_release(*lock);
    *lock = nullptr;
}

#define LOCK_RETAINER_(retainer, lock_)                   \
    struct lock *retainer CLEANUP(lock_retainer_release) = lock_; \
    lock_acquire(retainer);

#define LOCK_RETAINER(lock) \
    LOCK_RETAINER_(retainer, lock)
