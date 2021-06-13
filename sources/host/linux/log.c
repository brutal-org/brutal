#include <host/log.h>
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

static write_result_t host_log_write(struct writer *writer, char const *data, size_t size)
{
    UNUSED(writer);
    write(2, data, size);
    return OK(write_result_t, size);
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
