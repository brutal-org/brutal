#include <brutal/base.h>
#include <brutal/host/log.h>
#include <errno.h>
#include <execinfo.h>
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

static IoWriteResult host_log_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    if (write(2, data, size) == -1)
    {
        return ERR(IoWriteResult, make_error(ERR_KIND_UNDEFINED, strerror(errno)));
    }

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

void host_log_backtrace(void)
{
    void *array[256];

    int size = backtrace(array, 256);
    char **strings = backtrace_symbols(array, size);

    if (strings != NULL)
    {

        fprintf(stderr, "Obtained %d stack frames.\n", size);
        for (int i = 0; i < size; i++)
        {
            fprintf(stderr, "%s\n", strings[i]);
        }

        fflush(stderr);
    }

    free(strings);
}

void host_log_panic(void)
{
    host_log_backtrace();
    abort();
}
