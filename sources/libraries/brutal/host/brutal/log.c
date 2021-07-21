#include <brutal/alloc/global.h>
#include <brutal/ds/buffer.h>
#include <brutal/host/log.h>
#include <brutal/sync.h>
#include <syscalls/syscalls.h>

static bool initialized = false;
static Lock lock;
static Buffer buffer;
static IoWriter log;

void host_log_lock(void)
{
    lock_acquire(&lock);
}

static IoWriteResult host_log_write(MAYBE_UNUSED IoWriter *writer, char const *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        buffer_push(&buffer, data[i]);

        if (data[i] == '\n')
        {
            br_log((char const *)buffer_begin(&buffer), buffer_used(&buffer));
            buffer_clear(&buffer);
        }
    }

    return OK(IoWriteResult, size);
}

IoWriter *host_log_writer(void)
{
    if (!initialized)
    {
        log.write = host_log_write;
        buffer_init(&buffer, 128, alloc_global());
        initialized = true;
    }

    return &log;
}

void host_log_unlock(void)
{
    lock_release(&lock);
}

void host_log_panic(void)
{
    br_exit(BR_TASK_SELF, -1);
    __builtin_unreachable();
}
