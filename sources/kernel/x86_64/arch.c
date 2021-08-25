#include "kernel/arch.h"
#include "kernel/cpu.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/com.h"
#include "kernel/x86_64/smp.h"

static bool log_initialized = false;
static IoWriter log;

static IoWriteResult arch_debug_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    com_write(COM1, data, size);
    return OK(IoWriteResult, size);
}

IoWriter *arch_debug(void)
{
    if (!log_initialized)
    {
        log.write = arch_debug_write;
        log_initialized = true;
    }

    return &log;
}

void arch_idle(void)
{
    asm_hlt();
}

void arch_stop(void)
{
    smp_stop_all();

    while (true)
    {
        asm_cli();
        asm_hlt();
    }
}

void arch_yield(void)
{
    asm("int $100");
}
