#include "kernel/arch.h"
#include "kernel/cpu.h"
#include "kernel/x86_64/apic.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cmos.h"
#include "kernel/x86_64/com.h"
#include "kernel/x86_64/smp.h"

static IoResult arch_debug_write(MAYBE_UNUSED void *context, uint8_t const *data, size_t size)
{
    com_write(COM1, data, size);
    return OK(IoResult, size);
}

IoWriter arch_debug(void)
{
    return (IoWriter){.write = arch_debug_write};
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

uint32_t arch_in(uint16_t port, int size)
{
    if (size == 1)
    {
        return asm_in8(port);
    }
    else if (size == 2)
    {
        return asm_in16(port);
    }
    else if (size == 4)
    {
        return asm_in32(port);
    }
    else
    {
        return 0;
    }
}

void arch_out(uint16_t port, int size, uint32_t data)
{
    if (size == 1)
    {
        asm_out8(port, data);
    }
    else if (size == 2)
    {
        asm_out16(port, data);
    }
    else if (size == 4)
    {
        asm_out32(port, data);
    }
}

BrTimeStamp arch_now(void)
{
    return datetime_to_timestamp(cmos_read_rtc());
}
