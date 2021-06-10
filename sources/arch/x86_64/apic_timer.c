#include <stdint.h>
#include "apic.h"
#include "apic_timer.h"
#include "arch/mmio.h"
#include "pit.h"

void apic_timer_initialize(void)
{
    lapic_write(LAPIC_REG_TIMER_DIV, APIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_REG_TIMER_INITCNT, 0xFFFFFFFF);              /* Set the value to -1 */

    pit_sleep(10);

    lapic_write(LAPIC_REG_LVT_TIMER, LAPIC_REGISTER_LVT_INT_MASKED);

    uint32_t tick_in_10ms = 0xFFFFFFFF - lapic_read(LAPIC_REG_TIMER_CURRCNT);

    lapic_write(LAPIC_REG_LVT_TIMER, LAPIC_IRQ | LAPIC_LVT_TIMER_MODE_PERIODIC);
    lapic_write(LAPIC_REG_TIMER_DIV, APIC_TIMER_DIVIDE_BY_16);
    lapic_write(LAPIC_REG_TIMER_INITCNT, tick_in_10ms);
}