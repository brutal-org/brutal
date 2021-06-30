#include <brutal/base.h>
#include <host/log.h>
#include <stdlib.h>
#include <unistd.h>

static bool log_initialized = false;
static struct writer log;

void host_log_lock(void)
{
    // FIXME: no-op
}

void host_log_unlock(void)
{
    // FIXME: no-op
}

static WriteResult host_log_write(MAYBE_UNUSED struct writer *writer, char const *data, size_t size)
{
    write(2, data, size);
    return OK(WriteResult, size);
}

struct writer *host_log_writer(void)
{
    if (!log_initialized)
    {
        log.write = host_log_write;
        log_initialized = true;
    }

    return &log;
}

void host_log_panic(void)
{
    abort();
}
