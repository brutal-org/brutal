#include <library/base.h>

#include "arch/arch.h"
#include "arch/com.h"

static bool log_initialized = false;
static struct writer log;

static write_r arch_log_write(struct writer *writer, const char *data, size_t size)
{
    UNUSED(writer);

    com_write(COM1, data, size);
    return (write_r)OK(size);
}

struct writer *arch_log(void)
{
    if (!log_initialized)
    {
        log.write = arch_log_write;
        log_initialized = true;
    }

    return &log;
}
