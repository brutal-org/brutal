#include <acpi/hpet.h>
#include <acpi/rsdt.h>
#include <brutal-debug>
#include <brutal-mem>

#include "kernel/x86_64/hpet.h"

#include "kernel/mmap.h"

static uintptr_t _base = 0;
static uint64_t _clock = 0;

void hpet_write(uintptr_t reg, uint64_t value)
{
    volatile_write64(reg + _base, value);
}

uint64_t hpet_read(uintptr_t reg)
{
    return volatile_read64(reg + _base);
}

void hpet_initialize(Acpi *acpi)
{
    AcpiHpet *hpet_table = (AcpiHpet *)acpi_rsdt_lookup_first(acpi, ACPI_HPET_SIG);

    _base = mmap_phys_to_io(hpet_table->address);

    if (hpet_table->address_space_id == HPET_ADDRESS_SPACE_IO)
    {
        panic$("Unsupported Hpet address space");
    }

    _clock = hpet_read(HPET_GENERAL_CAPABILITIES) >> HPET_CAP_COUNTER_CLOCK_OFFSET;

    hpet_write(HPET_GENERAL_CONFIGUATION, HPET_CONF_TURN_OFF);
    hpet_write(HPET_MAIN_COUNTER_VALUE, 0);
    hpet_write(HPET_GENERAL_CONFIGUATION, HPET_CONF_TURN_ON);
}

void hpet_sleep(int ms)
{
    uint64_t target = hpet_read(HPET_MAIN_COUNTER_VALUE) + (ms * 1000000000000) / _clock;
    wait_for$(hpet_read(HPET_MAIN_COUNTER_VALUE) >= target);
}
