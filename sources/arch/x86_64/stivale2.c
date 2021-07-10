#include <brutal/base.h>
#include <brutal/log.h>
#include <brutal/mem.h>
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

[[gnu::section(".stivale2hdr"), gnu::used]] static struct stivale2_header stivale_hdr = {
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

void fill_handover_framebuffer(struct handover *target, struct stivale2_struct_tag_framebuffer *framebuffer)
{
    target->framebuffer.has_framebuffer = true;
    target->framebuffer.framebuffer_width = framebuffer->framebuffer_width;
    target->framebuffer.framebuffer_height = framebuffer->framebuffer_height;
    target->framebuffer.framebuffer_height = framebuffer->framebuffer_bpp;
    target->framebuffer.framebuffer_physical_addr = framebuffer->framebuffer_addr;
}

void fill_handover_modules(struct handover *target, struct stivale2_struct_tag_modules *modules)
{
    target->modules.module_count = modules->module_count;

    for (size_t i = 0; i < MIN(modules->module_count, MAX_MODULE_COUNT); i++)
    {
        target->modules.module[i].addr = modules->modules[i].begin;
        target->modules.module[i].size = modules->modules[i].end - modules->modules[i].begin;

        target->modules.module[i].module_name = str_cast_fix(StrFix128, modules->modules[i].string);
    }
}

void stivale2_entry(struct stivale2_struct *info)
{
    log("Booting from a stivale2 bootloader...");

    static struct handover handover =
        {
            .identifier = HANDOVER_IDENTIFIER,
        };

    auto memory_map = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    if (memory_map)
    {
        fill_handover_mmap(&handover, memory_map);
    }

    auto rsdp = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_RSDP_ID);

    if (rsdp)
    {
        fill_handover_rsdp(&handover, rsdp);
    }

    auto framebuffer = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    if (framebuffer)
    {
        fill_handover_framebuffer(&handover, framebuffer);
    }

    auto modules = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MODULES_ID);

    if (modules)
    {
        fill_handover_modules(&handover, modules);
    }

    arch_entry_main(&handover);

    for (;;)
    {
        asm_cli();
        asm_hlt();
    }
}
