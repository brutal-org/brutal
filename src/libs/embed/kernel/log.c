#include <embed/log.h>
#include <brutal-sync>
#include "kernel/arch.h"

static Lock _lock;

void embed_log_lock(void)
{
    lock_acquire(&_lock);
}

IoWriter embed_log_writer(void)
{
    return arch_debug();
}

void embed_log_unlock(void)
{
    lock_release(&_lock);
}

void embed_log_panic(void)
{
    arch_stop();
    __builtin_unreachable();
}
