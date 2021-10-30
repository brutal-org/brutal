#include <brutal/base.h>
#include <embed/log.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static bool log_initialized = false;
static IoWriter log;

void host_log_lock(void)
{
    // FIXME: no-op
}

void host_log_unlock(void)
{
    // FIXME: no-op
}

static IoResult host_log_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    if (write(2, data, size) == -1)
    {
        return ERR(IoResult, make_error(ERR_KIND_UNDEFINED, strerror(errno)));
    }

    return OK(IoResult, size);
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

void host_log_panic(void)
{
    abort();
}
