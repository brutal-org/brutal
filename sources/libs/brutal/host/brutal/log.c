#include <brutal/host/log.h>
#include <brutal/sync.h>
#include <syscalls/syscalls.h>

static Lock logger_lock;
static bool log_initialized = false;
static IoWriter log;

void host_log_lock(void)
{
    lock_acquire(&logger_lock);
}

static IoWriteResult host_log_write(MAYBE_UNUSED IoWriter *writer, char const *data, size_t size)
{
    br_log(data, size);
    return OK(IoWriteResult, size);
}

IoWriter *host_log_writer(void)
{
    if (!log_initialized)
    {
        log.write = host_log_write;
        log_initialized = true;
    }

    return &log;
}

void host_log_unlock(void)
{
    lock_release(&logger_lock);
}

void host_log_panic(void)
{
    br_exit(BR_TASK_SELF, -1);
    __builtin_unreachable();
}
