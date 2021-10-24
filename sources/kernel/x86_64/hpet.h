#pragma once

#include <acpi/base.h>

#define HPET_ADDRESS_SPACE_MEMORY 0
#define HPET_ADDRESS_SPACE_IO 1

#define HPET_CAP_COUNTER_CLOCK_OFFSET (32)

#define HPET_CONF_TURN_ON (1)
#define HPET_CONF_TURN_OFF (0)

enum hpet_registers
{
    HPET_GENERAL_CAPABILITIES = 0,
    HPET_GENERAL_CONFIGUATION = 16,
    HPET_MAIN_COUNTER_VALUE = 240,
};

void hpet_initialize(Acpi *acpi);

void hpet_sleep(int ms);
