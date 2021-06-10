#include <brutal/task/lock.h>

bool lock_try_acquire(struct lock *lock)
{
    auto result = __sync_bool_compare_and_swap(&lock->locked, 0, 1);
    __sync_synchronize();
    return result;
}

void lock_acquire(struct lock *lock)
{
    while (!lock_try_acquire(lock))
    {
        asm("pause");
    }

    __sync_synchronize();
}

void lock_release(struct lock *lock)
{
    __sync_synchronize();
    __atomic_store_n(&lock->locked, 0, __ATOMIC_SEQ_CST);
}
