#pragma once

#include <brutal-base>

typedef struct
{
    atomic_int locked;
} Lock;

bool lock_try_acquire(Lock *lock);

void lock_acquire(Lock *lock);

void lock_release(Lock *lock);

static inline void lock_retainer_release(Lock **lock)
{
    if (lock != nullptr)
    {
        lock_release(*lock);
        *lock = nullptr;
    }
}

#define LOCK_RETAINER_(retainer, lock)                           \
    typeof(lock) retainer CLEANUP(lock_retainer_release) = lock; \
    lock_acquire(retainer);

#define LOCK_RETAINER(lock) \
    LOCK_RETAINER_(concat$(retainer, __COUNTER__), lock)

#define LOCK_RETAINER_TRY_(retainer, lock)                       \
    typeof(lock) retainer CLEANUP(lock_retainer_release) = lock; \
    if (lock_try_acquire(retainer) || (retainer = nullptr))

#define LOCK_RETAINER_TRY(lock) \
    LOCK_RETAINER_TRY_(concat$(retainer, __COUNTER__), lock)
