#include <lock.h>

void lock_acquire(struct lock *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
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