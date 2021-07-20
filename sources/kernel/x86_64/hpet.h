#pragma once

#include <brutal/base.h>
#include <handover/handover.h>
#include "kernel/x86_64/acpi.h"

enum hpet_registers
{
    HPET_GENERAL_CAPABILITIES = 0,
    HPET_GENERAL_CONFIGUATION = 16,
    HPET_MAIN_COUNTER_VALUE = 240,
};

void hpet_initialize(Handover *handover);

void hpet_sleep(int ms);
