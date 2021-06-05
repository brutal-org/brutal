#include <library/base.h>
#include <library/log.h>

#include "arch/arch.h"
#include "arch/asm.h"
#include "arch/stivale2.h"

static uint8_t stack[4096 * 4];

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0,
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0,
};

void stivale2_entry(struct stivale2_struct *info);

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    .entry_point = (uintptr_t)stivale2_entry,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1),
    .tags = (uintptr_t)&framebuffer_hdr_tag,
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;

    while (current_tag)
    {
        if (current_tag->identifier == id)
        {
            return current_tag;
        }

        current_tag = (void *)current_tag->next;
    }

    return nullptr;
}

static enum handover_mmap_entry_type stivale_mmap_type_to_handover_type(int stivale_entry)
{
    switch (stivale_entry)
    {

    case STIVALE2_MMAP_USABLE:
        return HANDOVER_MMAP_FREE;

    case STIVALE2_MMAP_ACPI_NVS:
    case STIVALE2_MMAP_RESERVED:
    case STIVALE2_MMAP_BAD_MEMORY:
        return HANDOVER_MMAP_RESERVED;

    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
    case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        return HANDOVER_MMAP_RECLAIMABLE; // may be used after everything is loaded

    case STIVALE2_MMAP_FRAMEBUFFER:
        return HANDOVER_MMAP_FRAMEBUFFER;

    default:
        return HANDOVER_MMAP_RESERVED; // by default not detected entry are unused
    }
}

static void fill_handover_mmap(struct handover *target, struct stivale2_struct_tag_memmap *memory_map)
{
    target->mmap.size = memory_map->entries;
    for (size_t i = 0; i < memory_map->entries; i++)
    {
        target->mmap.mmap_table[i].length = memory_map->memmap[i].length;
        target->mmap.mmap_table[i].base = memory_map->memmap[i].base;
        target->mmap.mmap_table[i].type = stivale_mmap_type_to_handover_type(memory_map->memmap[i].type);
    }
}

void stivale2_entry(struct stivale2_struct *info)
{
    UNUSED(info);

    log("Booting from a stivale2 bootloader...");

    struct handover handover = {};

    fill_handover_mmap(&handover, stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    arch_entry(&handover);

    for (;;)
    {
        cli();
        hlt();
    }
}
