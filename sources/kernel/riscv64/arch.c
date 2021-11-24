#include "kernel/arch.h"
#include "kernel/riscv64/arch.h"

static bool log_initialized = false;
static IoWriter log;

void set_arch_uart_device(GenericUartDevice *device)
{
    log_initialized = true;
    log = uart_writer(device);
}

IoWriter *arch_debug(void)
{
    if (!log_initialized)
    {
        log.write = NULL;
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
