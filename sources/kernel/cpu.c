#include <library/log.h>
#include "arch/cpu.h"
#include "kernel/cpu.h"

void cpu_found(int id)
{
    log("Found CPU {}!", id);

    arch_cpu(id)->id = id;
    arch_cpu(id)->present = true;
}

struct cpu *cpu_self(void)
{
    return arch_cpu_this();
}
