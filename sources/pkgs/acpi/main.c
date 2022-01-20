#include <acpi/base.h>
#include <acpi/rsdt.h>
#include <bal/boot.h>
#include <brutal/debug.h>
#include <ipc/ipc.h>

Iter dump_sdth(AcpiSdth *sdth, void *)
{
    log$("- '{}'", acpi_sdth_sig(sdth));
    return ITER_CONTINUE;
}

int ipc_component_main(IpcComponent *self)
{
    Acpi acpi = {};
    acpi_init(&acpi, handover->rsdp);

    log$("RSDT Dump:");
    acpi_rsdt_iterate(&acpi, (IterFn *)dump_sdth, nullptr);

    return ipc_component_run(self);
}
