#include <brutal/sync/lock.h>

bool lock_try_acquire(struct lock *lock)
{
    bool expected = false;
    auto result = atomic_compare_exchange_strong(&lock->locked, &expected, true);
    atomic_thread_fence(memory_order_seq_cst);
    return result;
}

void lock_acquire(struct lock *lock)
{
    while (!lock_try_acquire(lock))
    {
        asm("pause");
    }

    atomic_thread_fence(memory_order_seq_cst);
}

void lock_release(struct lock *lock)
{
    atomic_thread_fence(memory_order_seq_cst);
    lock->locked = false;
}
