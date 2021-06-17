#include "arch/arch.h"
#include "arch/cpu.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/com.h"

static bool log_initialized = false;
static struct writer log;

static write_result_t arch_debug_write(MAYBE_UNUSED struct writer *writer, char const *data, size_t size)
{
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

void arch_resched_other(void)
{
    for (size_t i = 1; i < cpu_count(); i++)
    {
        apic_send_ipit(i, IPIT_RESCHED);
    }
}

void arch_idle(void)
{
    while (true)
    {
        asm_hlt();
    }
}

void arch_enable_interrupt()
{
    asm_sti();
}

void arch_disable_interrupt()
{
    asm_cli();
}

void arch_cpu_pause()
{
    asm_pause();
}

void arch_cpu_halt()
{
    asm_hlt();
}
