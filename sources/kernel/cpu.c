#include <library/log.h>

#include "arch/cpu.h"
#include "kernel/cpu.h"

static size_t cpu_count = 0;

void cpu_found(int id)
{
    log("Found CPU {}!", id);

    arch_cpu(id)->id = id;
    arch_cpu(id)->present = true;

    cpu_count++;
}
