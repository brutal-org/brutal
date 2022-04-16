#include <brutal-debug>
#include <embed/sync.h>

#include "rwlock.h"

void rwlock_acquire_read(RwLock *self)
{
    embed_sync_enter();

    while (!rwlock_try_acquire_read(self))
    {
#ifdef EMBED_HAS_PAUSE
        embed_sync_pause();
#endif
        atomic_thread_fence(memory_order_seq_cst);
    }
}

bool rwlock_try_acquire_read(RwLock *self)
{
    LOCK_RETAINER(&self->lock);

    if (self->pendings)
    {
        return false;
    }

    if (self->writers)
    {
        return false;
    }

    self->readers++;

    embed_sync_enter();

    return true;
}

void rwlock_release_read(RwLock *self)
{
    LOCK_RETAINER(&self->lock);

    assert_greater_than(self->readers--, 0);

    embed_sync_leave();
}

void rwlock_acquire_write(RwLock *self)
{
    embed_sync_enter();

    self->pendings++;

    while (!rwlock_try_acquire_write(self))
    {
#ifdef EMBED_HAS_PAUSE
        embed_sync_pause();
#endif
        atomic_thread_fence(memory_order_seq_cst);
    }

    self->pendings--;
}

bool rwlock_try_acquire_write(RwLock *self)
{
    LOCK_RETAINER(&self->lock);

    if (self->readers)
    {
        return false;
    }

    if (self->writers)
    {
        return false;
    }

    self->writers++;

    embed_sync_enter();
    return true;
}

void rwlock_release_write(RwLock *self)
{
    LOCK_RETAINER(&self->lock);

    assert_greater_than(self->writers--, 0);
    embed_sync_leave();
}
