#include <host/log.h>

#include "arch/arch.h"

void host_log_lock(void)
{
}

struct writer *host_log_writer(void)
{
    return arch_debug();
}

void host_log_unlock(void)
{
}