#include <brutal/sync.h>
#include <embed/log.h>
#include "kernel/arch.h"

static Lock _lock;

void host_log_lock(void)
{
    lock_acquire(&_lock);
}

IoWriter *host_log_writer(void)
{
    return arch_debug();
}

void host_log_unlock(void)
{
    lock_release(&_lock);
}

void host_log_panic(void)
{
    arch_stop();
    __builtin_unreachable();
}
