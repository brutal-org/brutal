#include "kernel/cpu.h"
#include <brutal-debug>

void cpu_begin_interrupt(void)
{
    assert_truth(cpu_self()->retain_enable);
    cpu_self()->retain_enable = false;
}

void cpu_end_interrupt(void)
{
    assert_falsity(cpu_self()->retain_enable);
    cpu_self()->retain_enable = true;
}

void cpu_retain_disable(void)
{
    cpu_self()->retain_enable = false;
}

void cpu_retain_enable(void)
{
    cpu_self()->retain_enable = true;
}

void cpu_retain_interrupts(void)
{
    if (cpu_self()->retain_enable)
    {
        cpu_disable_interrupts();
        cpu_self()->retain_depth++;
    }
}

void cpu_release_interrupts(void)
{
    if (cpu_self()->retain_enable)
    {
        cpu_self()->retain_depth--;

        if (cpu_self()->retain_depth == 0)
        {
            cpu_enable_interrupts();
        }
    }
}
