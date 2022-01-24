#include <brutal/debug.h>
#include "init/bus.h"

void bus_init(Bus *bus, Handover *handover, Alloc *alloc)
{
    *bus = (Bus){};

    bus->handover = handover;
    vec_init(&bus->caps, alloc);
}

void bus_deinit(Bus *bus)
{
    vec_deinit(&bus->caps);
}

IpcCap bus_lookup(Bus *bus, IpcProto proto)
{
    vec_foreach_v(cap, &bus->caps)
    {
        if (cap.proto == proto)
        {
            return cap;
        }
    }

    return (IpcCap){};
}

void bus_start(Bus *bus, Str name, IpcCap *caps, size_t len)
{
    log$("Starting service '{case:pascal}'...", name);

    HandoverModule const *elf = handover_find_module(bus->handover, name);
    assert_not_null(elf);

    BalMem elf_mem;
    bal_mem_init_pmm(&elf_mem, elf->addr, elf->size);

    BalTask elf_task;
    bal_task_init(&elf_task, name);

    bal_task_exec(&elf_task, &elf_mem, BR_RIGHT_IO | BR_RIGHT_LOG | BR_RIGHT_PMM | BR_RIGHT_IRQ, caps, len);

    bal_mem_deinit(&elf_mem);
}
