#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>

struct lock
{
    int locked;
};

bool lock_try_acquire(struct lock *lock);

void lock_acquire(struct lock *lock);

void lock_release(struct lock *lock);

static inline void lock_retainer_release(struct lock **lock)
{
    if (lock != nullptr)
    {
        lock_release(*lock);
        *lock = nullptr;
    }
}

#define LOCK_RETAINER_(retainer, lock_)                   \
    struct lock *retainer CLEANUP(lock_retainer_release) = lock_; \
    lock_acquire(retainer);

#define LOCK_RETAINER(lock) \
    LOCK_RETAINER_(retainer, lock)

#define LOCK_RETAINER_TRY_(retainer, lock)               \
    auto retainer CLEANUP(lock_retainer_release) = lock; \
    if (lock_try_acquire(retainer) || (retainer = nullptr))

#define LOCK_RETAINER_TRY(lock) \
    LOCK_RETAINER_TRY_(retainer, lock)
