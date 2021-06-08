#include <library/log.h>

#include "arch/cpu.h"
#include "kernel/cpu.h"

void cpu_found(int id)
{
    log("Found CPU {}!", id);

    arch_cpu(id)->id = id;
    arch_cpu(id)->present = true;
}

void cpu_start(int id)
{
    log("loaded CPU {}!", id);

    while (true)
    {
    }
}