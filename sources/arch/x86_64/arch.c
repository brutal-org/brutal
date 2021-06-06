#include "arch/arch.h"

#include "arch/x86_64/com.h"

static bool log_initialized = false;
static struct writer log;

static write_result_t arch_debug_write(struct writer *writer, char const *data, size_t size)
{
    UNUSED(writer);

    com_write(COM1, data, size);
    return OK(write_result_t, size);
}

struct writer *arch_debug(void)
{
    if (!log_initialized)
    {
        log.write = arch_debug_write;
        log_initialized = true;
    }

    return &log;
}
