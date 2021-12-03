#include <bal/boot/handover.h>
#include <brutal/debug.h>

HandoverModule const *handover_find_module(Handover const *handover, Str name)
{
    for (size_t i = 0; i < handover->modules.size; i++)
    {
        if (str_eq(name, str$(&handover->modules.module[i].module_name)))
        {
            return &handover->modules.module[i];
        }
    }

    return nullptr;
}

void handover_dump(Handover const *handover)
{
    if (handover->tag != HANDOVER_TAG)
    {
        log$("not valid handover tag: {}", handover->tag);
    }


    log$("Memory Map:");

    for (size_t i = 0; i < handover->mmap.size; i++)
    {
        HandoverMmapEntry entry = handover->mmap.entries[i];

        log$("\t{#p}-{#p} {}", entry.base, entry.base + entry.size - 1, ho_mmtype_to_str(entry.type));
    }

    log$("Framebuffer:");

    if (handover->framebuffer.present)
    {
        log$("\tAddress: {#p}", handover->framebuffer.addr);
        log$("\tWidth: {}", handover->framebuffer.width);
        log$("\tHeight: {}", handover->framebuffer.height);
        log$("\tPitch: {}", handover->framebuffer.pitch);
        log$("\tBpp: {}", handover->framebuffer.bpp);
    }
    else
    {
        log$("\tNot found!");
    }

    log$("Modules:");

    for (size_t i = 0; i < handover->modules.size; i++)
    {
        HandoverModule entry = handover->modules.module[i];

        log$("\t{#p}-{#p} {#}", entry.addr, entry.addr + entry.size, str$(&entry.module_name));
    }

    log$("Rsdp:");
    log$("\tAddress: {#p}", handover->rsdp);

    log$("Cmd lines:");

    if (handover->cmd_lines.present)
    {
        log$("\t value: '{}'", str$(&handover->cmd_lines.cmd_line));
    }
    else
    {
        log$("\tNot found!");
    }
}
