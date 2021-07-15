#pragma once

#include <brutal/base.h>
#include "arch/x86_64/acpi.h"
#include "kernel/handover.h"

enum hpet_registers
{
    HPET_GENERAL_CAPABILITIES = 0,
    HPET_GENERAL_CONFIGUATION = 16,
    HPET_MAIN_COUNTER_VALUE = 240,
};

void hpet_initialize(struct handover *handover);

void hpet_sleep(int ms);