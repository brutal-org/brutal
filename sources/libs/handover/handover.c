#include <brutal/log.h>
#include <handover/handover.h>

HandoverModule const *handover_find_module(Handover const *handover, Str name)
{
    for (size_t i = 0; i < handover->modules.size; i++)
    {
        if (str_eq(name, str_cast(&handover->modules.module[i].module_name)))
        {
            return &handover->modules.module[i];
        }
    }

    return nullptr;
}

void handover_dump(Handover const *handover)
{
    log("Memory Map:");

    for (size_t i = 0; i < handover->mmap.size; i++)
    {
        auto entry = handover->mmap.entries[i];

        log("\t{#p}-{#p} {}", entry.base, entry.base + entry.length - 1, entry.type);
    }

    log("Framebuffer:");

    if (handover->framebuffer.present)
    {
        log("\tAddress: {#p}", handover->framebuffer.addr);
        log("\tWidth: {}", handover->framebuffer.width);
        log("\tHeight: {}", handover->framebuffer.height);
        log("\tPitch: {}", handover->framebuffer.pitch);
        log("\tBpp: {}", handover->framebuffer.bpp);
    }
    else
    {
        log("\tNot found!");
    }

    log("Modules:");

    for (size_t i = 0; i < handover->modules.size; i++)
    {
        auto entry = handover->modules.module[i];

        log("\t{#p}-{#p} {#}", entry.addr, entry.addr + entry.size, str_cast(&entry.module_name));
    }

    log("Rsdp:");
    log("\tAddress: {#p}", handover->rsdp);
}
