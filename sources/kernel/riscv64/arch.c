#include "kernel/arch.h"

void arch_idle(void)
{
    while (true)
    {
    }
}

void arch_stop(void)
{
    while (true)
    {
    }
}

void arch_yield(void)
{
}

uint32_t arch_in(MAYBE_UNUSED uint16_t port, MAYBE_UNUSED int size)
{
    return 0;
}

void arch_out(MAYBE_UNUSED uint16_t port, MAYBE_UNUSED int size, MAYBE_UNUSED uint32_t data)
{
}

IoWriter *arch_debug(void)
{
    return nullptr;
}
