#include "kernel/arch.h"
#include "kernel/riscv64/uart.h"

static bool log_initialized = false;
static IoWriter log;

static IoResult arch_debug_write(MAYBE_UNUSED void *context, uint8_t const *data, MAYBE_UNUSED size_t offset, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        uart_putc(data[i]);
    }

    return OK(IoResult, size);
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
