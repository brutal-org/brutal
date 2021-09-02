#include <acpi/acpi.h>
#include <acpi/rsdt.h>
#include <brutal/log.h>
#include <handover/handover.h>
#include <syscalls/syscalls.h>

Iter dump_sdth(AcpiSdth *sdth, void *)
{
    log$("- '{}'", acpi_sdth_sig(sdth));
    return ITER_CONTINUE;
}

int br_entry_handover(Handover *handover)
{
    BrCreateArgs acpi_mem_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = 0,
            .size = 0x100000000,
            .flags = BR_MEM_OBJ_PMM,
        },
    };

    assert_truth(br_create(&acpi_mem_obj) == BR_SUCCESS);

    BrMapArgs acpi_mem_map = {
        .space = BR_SPACE_SELF,
        .mem_obj = acpi_mem_obj.handle,
    };

    assert_truth(br_map(&acpi_mem_map) == BR_SUCCESS);

    Acpi acpi = {};
    acpi_init(&acpi, acpi_mem_map.vaddr, handover->rsdp);

    log$("RSDT Dump:");
    acpi_rsdt_iterate(&acpi, (IterFn *)dump_sdth, nullptr);

    while (true)
    {
    }

    return 0;
}
