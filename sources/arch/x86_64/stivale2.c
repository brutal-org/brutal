#include <brutal/base.h>
#include <brutal/log.h>
#include "arch/arch.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/memory/mmap.h"
#include "arch/x86_64/stivale2.h"
#include "kernel/constants.h"

static uint8_t stack[KERNEL_STACK_SIZE];

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

[[gnu::section(".stivale2hdr")]] [[gnu::used]] static struct stivale2_header stivale_hdr = {
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

    case STIVALE2_MMAP_KERNEL_AND_MODULES:
        return HANDOVER_MMAP_KERNEL_MODULE;

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
        auto entry = &target->mmap.entries[i];

        entry->length = memory_map->memmap[i].length;
        entry->base = memory_map->memmap[i].base;
        entry->type = stivale_mmap_type_to_handover_type(memory_map->memmap[i].type);
    }
}

static void fill_handover_rsdp(struct handover *target, struct stivale2_struct_tag_rsdp *rsdp)
{
    target->rsdp = mmap_io_to_phys(rsdp->rsdp);
}

void stivale2_entry(struct stivale2_struct *info)
{
    log("Booting from a stivale2 bootloader...");

    struct handover handover = {};

    fill_handover_mmap(&handover, stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    fill_handover_rsdp(&handover, stivale2_get_tag(info, STIVALE2_STRUCT_TAG_RSDP_ID));

    arch_entry_main(&handover);

    for (;;)
    {
        asm_cli();
        asm_hlt();
    }
}
