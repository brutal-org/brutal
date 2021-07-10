#include <brutal/base.h>
#include <brutal/host/log.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
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

static IoWriteResult host_log_write(MAYBE_UNUSED IoWriter *writer, char const *data, size_t size)
{
    write(2, data, size);
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
