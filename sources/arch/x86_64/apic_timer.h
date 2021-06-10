#pragma once 

#define IRQ0 32
#define LAPIC_LVT_TIMER_MODE_PERIODIC 0x20000

enum apic_timer_division /* Intel Volume 3A 10.5.4 */
{
    APIC_TIMER_DIVIDE_BY_2 = 0,
    APIC_TIMER_DIVIDE_BY_4 = 1,
    APIC_TIMER_DIVIDE_BY_8 = 2,
    APIC_TIMER_DIVIDE_BY_16 = 3,
    APIC_TIMER_DIVIDE_BY_32 = 4,
    APIC_TIMER_DIVIDE_BY_64 = 5,
    APIC_TIMER_DIVIDE_BY_128 = 6,
    APIC_TIMER_DIVIDE_BY_1 = 7
};

void apic_timer_initialize(void);