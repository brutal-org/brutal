#include <stivale/stivale.h>

void *stivale2_get_tag(struct stivale2_struct const *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (struct stivale2_tag *)stivale2_struct->tags;

    while (current_tag)
    {
        if (current_tag->identifier == id)
        {
            return current_tag;
        }

        current_tag = (struct stivale2_tag *)current_tag->next;
    }

    return nullptr;
}

static HandoverMmapType stivale_mmap_type_to_handover_type(int stivale_entry)
{
    switch (stivale_entry)
    {

    case STIVALE2_MMAP_USABLE:
        return HANDOVER_MMAP_FREE;

    case STIVALE2_MMAP_KERNEL_AND_MODULES:
        return HANDOVER_MMAP_USED;

    case STIVALE2_MMAP_ACPI_NVS:
    case STIVALE2_MMAP_RESERVED:
    case STIVALE2_MMAP_BAD_MEMORY:
    case STIVALE2_MMAP_FRAMEBUFFER:
        return HANDOVER_MMAP_RESERVED;

    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
    case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        return HANDOVER_MMAP_RECLAIMABLE;

    default:
        return HANDOVER_MMAP_RESERVED;
    }
}

static void fill_handover_mmap(Handover *target, struct stivale2_struct_tag_memmap const *memory_map)
{
    target->mmap.size = memory_map->entries;

    for (size_t i = 0; i < memory_map->entries; i++)
    {
        HandoverMmapEntry *entry = &target->mmap.entries[i];

        entry->size = memory_map->memmap[i].length;
        entry->base = memory_map->memmap[i].base;
        entry->type = stivale_mmap_type_to_handover_type(memory_map->memmap[i].type);
    }
}

static void fill_handover_rsdp(Handover *target, struct stivale2_struct_tag_rsdp const *rsdp)
{
    target->rsdp = rsdp->rsdp;
}

static void fill_handover_framebuffer(Handover *target, struct stivale2_struct_tag_framebuffer const *framebuffer)
{
    target->framebuffer.present = true;
    target->framebuffer.addr = framebuffer->framebuffer_addr;
    target->framebuffer.width = framebuffer->framebuffer_width;
    target->framebuffer.height = framebuffer->framebuffer_height;
    target->framebuffer.pitch = framebuffer->framebuffer_pitch;
    target->framebuffer.bpp = framebuffer->framebuffer_bpp;
}

void fill_handover_modules(Handover *target, struct stivale2_struct_tag_modules const *modules)
{
    target->modules.size = modules->module_count;

    for (size_t i = 0; i < m_min(modules->module_count, MAX_MODULE_COUNT); i++)
    {
        target->modules.module[i].addr = modules->modules[i].begin;
        target->modules.module[i].size = align_up$(modules->modules[i].end - modules->modules[i].begin, MEM_PAGE_SIZE);

        target->modules.module[i].module_name = str_fix$(StrFix32, modules->modules[i].string);
    }
}

void stivale2_copy_to_handover(struct stivale2_struct const *info, Handover *handover)
{
    handover->tag = HANDOVER_TAG;

    void *memory_map = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    if (memory_map)
    {
        fill_handover_mmap(handover, (struct stivale2_struct_tag_memmap const *)memory_map);
    }

    void *rsdp = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_RSDP_ID);

    if (rsdp)
    {
        fill_handover_rsdp(handover, (struct stivale2_struct_tag_rsdp const *)rsdp);
    }

    void *framebuffer = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    if (framebuffer)
    {
        fill_handover_framebuffer(handover, (struct stivale2_struct_tag_framebuffer const *)framebuffer);
    }

    void *modules = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MODULES_ID);

    if (modules)
    {
        fill_handover_modules(handover, (struct stivale2_struct_tag_modules const *)modules);
    }
}
