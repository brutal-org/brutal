#include <brutal/mem.h>
#include "kernel/mmap.h"
#include "kernel/x86_64/hpet.h"

static uintptr_t hpet_base = 0;
static uint64_t hpet_clock = 0;

void hpet_initialize(Handover *handover)
{
    struct acpi_hpet *hpet_table = acpi_find_hpet(handover->rsdp);
    hpet_base = mmap_phys_to_io(hpet_table->address);

    hpet_clock = volatile_read64(hpet_base + HPET_GENERAL_CAPABILITIES) >> 32;

    volatile_write64(hpet_base + HPET_GENERAL_CONFIGUATION, 0);
    volatile_write64(hpet_base + HPET_MAIN_COUNTER_VALUE, 0);
    volatile_write64(hpet_base + HPET_GENERAL_CONFIGUATION, 1);
}

void hpet_sleep(int ms)
{
    uint64_t target = volatile_read64(hpet_base + HPET_MAIN_COUNTER_VALUE) + (ms * 1000000000000) / hpet_clock;
    WAIT_FOR(volatile_read64(hpet_base + HPET_MAIN_COUNTER_VALUE) >= target);
}
