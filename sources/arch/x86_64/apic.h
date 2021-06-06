#pragma once

#include "kernel/handover.h"

enum lapic_reg
{
    LAPIC_REG_EOI = 0x0b0,
    LAPIC_REG_SPURIOUS = 0x0f0,
    LAPIC_REG_ICR0 = 0x300,
    LAPIC_REG_ICR1 = 0x310,
};

void apic_initalize(struct handover const *handover);

void apic_eoi(void);
