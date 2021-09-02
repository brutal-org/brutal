#include <acpi/hpet.h>
#include <acpi/rsdt.h>
#include <brutal/log.h>
#include <brutal/mem.h>
#include "brutal/mem/volatile.h"
#include "kernel/mmap.h"
#include "kernel/x86_64/hpet.h"

static uintptr_t hpet_base = 0;
static uint64_t hpet_clock = 0;

void hpet_write(uintptr_t reg, uint64_t value)
{
    volatile_write64(reg + hpet_base, value);
}

uint64_t hpet_read(uintptr_t reg)
{
    return volatile_read64(reg + hpet_base);
}

void hpet_initialize(Acpi *acpi)
{
    AcpiHpet *hpet_table = (AcpiHpet *)acpi_rsdt_lookup_first(acpi, ACPI_HPET_SIG);

    hpet_base = mmap_phys_to_io(hpet_table->address);

    if (hpet_table->address_space_id == HPET_ADDRESS_SPACE_IO)
    {
        panic$("Unsupported Hpet address space");
    }

    hpet_clock = hpet_read(HPET_GENERAL_CAPABILITIES) >> HPET_CAP_COUNTER_CLOCK_OFFSET;

    hpet_write(HPET_GENERAL_CONFIGUATION, HPET_CONF_TURN_OFF);
    hpet_write(HPET_MAIN_COUNTER_VALUE, 0);
    hpet_write(HPET_GENERAL_CONFIGUATION, HPET_CONF_TURN_ON);
}

void hpet_sleep(int ms)
{
    uint64_t target = hpet_read(HPET_MAIN_COUNTER_VALUE) + (ms * 1000000000000) / hpet_clock;
    WAIT_FOR(hpet_read(HPET_MAIN_COUNTER_VALUE) >= target);
}
