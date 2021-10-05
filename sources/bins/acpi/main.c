#include <acpi/acpi.h>
#include <acpi/rsdt.h>
#include <bal/syscalls.h>
#include <brutal/log.h>
#include <handover/handover.h>

Iter dump_sdth(AcpiSdth *sdth, void *)
{
    log$("- '{}'", acpi_sdth_sig(sdth));
    return ITER_CONTINUE;
}

int br_entry_handover(Handover *handover)
{
    Acpi acpi = {};
    acpi_init(&acpi, handover->rsdp);

    log$("RSDT Dump:");
    acpi_rsdt_iterate(&acpi, (IterFn *)dump_sdth, nullptr);

    while (true)
    {
    }

    return 0;
}
