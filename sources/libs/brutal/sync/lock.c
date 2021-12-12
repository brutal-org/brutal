#include <brutal/sync/lock.h>
#include <embed/arch.h>
#include <embed/sync.h>

static bool lock_try_acquire_impl(Lock *lock)
{
    int expected = false;
    int result = atomic_compare_exchange_strong(&lock->locked, &expected, true);

    atomic_thread_fence(memory_order_seq_cst);

    return result;
}

bool lock_try_acquire(Lock *lock)
{
    embed_enter_critical_section();

    int result = lock_try_acquire_impl(lock);

    if (!result)
    {
        embed_leave_critical_section();
    }

    return result;
}

void lock_acquire(Lock *lock)
{
    embed_enter_critical_section();

    while (!lock_try_acquire_impl(lock))
    {
#ifdef ARCH_HAS_PAUSE
        arch_pause();
#endif
        atomic_thread_fence(memory_order_seq_cst);
    }

    atomic_thread_fence(memory_order_seq_cst);
}

void lock_release(Lock *lock)
{
    atomic_thread_fence(memory_order_seq_cst);
    lock->locked = false;

    embed_leave_critical_section();
}
