#include <embed/sync.h>

#include "lock.h"

static bool lock_try_acquire_impl(Lock *lock)
{
    int expected = false;
    int result = atomic_compare_exchange_strong(&lock->locked, &expected, true);

    atomic_thread_fence(memory_order_seq_cst);

    return result;
}

bool lock_try_acquire(Lock *lock)
{
    embed_sync_enter();

    int result = lock_try_acquire_impl(lock);

    if (!result)
    {
        embed_sync_leave();
    }

    return result;
}

void lock_acquire(Lock *lock)
{
    embed_sync_enter();

    while (!lock_try_acquire_impl(lock))
    {
#ifdef EMBED_HAS_PAUSE
        embed_sync_pause();
#endif
        atomic_thread_fence(memory_order_seq_cst);
    }

    atomic_thread_fence(memory_order_seq_cst);
}

void lock_release(Lock *lock)
{
    atomic_thread_fence(memory_order_seq_cst);
    lock->locked = false;

    embed_sync_leave();
}
