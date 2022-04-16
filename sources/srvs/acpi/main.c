#include <acpi/rsdt.h>
#include <protos/boot.h>
#include <brutal-alloc>
#include <brutal-debug>

Iter dump_sdth(AcpiSdth *sdth, void *)
{
    log$("- '{}'", acpi_sdth_sig(sdth));
    return ITER_CONTINUE;
}

int ipc_component_main(IpcComponent *self)
{
    IpcCap infos = ipc_component_require(self, IPC_BOOT_INFO_PROTO);

    uintptr_t rsdp = 0;
    if (boot_info_rsdp_rpc(self, infos, &rsdp, alloc_global()) != IPC_SUCCESS)
    {
        panic$("No rsdp found!");
    }

    Acpi acpi = {};
    acpi_init(&acpi, rsdp);

    log$("RSDT Dump:");
    acpi_rsdt_iterate(&acpi, (IterFn *)dump_sdth, nullptr);

    return ipc_component_run(self);
}
