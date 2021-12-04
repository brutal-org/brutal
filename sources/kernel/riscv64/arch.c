#include "kernel/arch.h"
#include "kernel/riscv64/arch.h"

static bool _init = false;
static IoWriter _lock;

void arch_use_uart(Uart *device)
{
    _init = true;
    _lock = uart_writer(device);
}

IoWriter *arch_debug(void)
{
    if (!_init)
    {
        _lock.write = NULL;
        _init = true;
    }

    return &_lock;
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
