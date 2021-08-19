#include <brutal/log.h>
#include <brutal/mem.h>
#include "kernel/arch.h"
#include "kernel/kernel.h"
#include "kernel/mmap.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/stivale2.h"

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

void stivale2_entry(struct stivale2_struct const *info);

[[gnu::section(".stivale2hdr"), gnu::used]] static struct stivale2_header stivale_hdr = {
    .entry_point = (uintptr_t)stivale2_entry,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1),
    .tags = (uintptr_t)&framebuffer_hdr_tag,
};

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
        return HANDOVER_MMAP_KERNEL_MODULE;

    case STIVALE2_MMAP_ACPI_NVS:
    case STIVALE2_MMAP_RESERVED:
    case STIVALE2_MMAP_BAD_MEMORY:
        return HANDOVER_MMAP_RESERVED;

    case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
    case STIVALE2_MMAP_ACPI_RECLAIMABLE:
        return HANDOVER_MMAP_RECLAIMABLE;

    case STIVALE2_MMAP_FRAMEBUFFER:
        return HANDOVER_MMAP_FRAMEBUFFER;

    default:
        return HANDOVER_MMAP_RESERVED;
    }
}

static void fill_handover_mmap(Handover *target, struct stivale2_struct_tag_memmap const *memory_map)
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

static void fill_handover_rsdp(Handover *target, struct stivale2_struct_tag_rsdp const *rsdp)
{
    target->rsdp = mmap_io_to_phys(rsdp->rsdp);
}

static void fill_handover_framebuffer(Handover *target, struct stivale2_struct_tag_framebuffer const *framebuffer)
{
    target->framebuffer.present = true;
    target->framebuffer.addr = mmap_io_to_phys(framebuffer->framebuffer_addr);
    target->framebuffer.width = framebuffer->framebuffer_width;
    target->framebuffer.height = framebuffer->framebuffer_height;
    target->framebuffer.pitch = framebuffer->framebuffer_pitch;
    target->framebuffer.bpp = framebuffer->framebuffer_bpp;
}

void fill_handover_modules(Handover *target, struct stivale2_struct_tag_modules const *modules)
{
    target->modules.size = modules->module_count;

    for (size_t i = 0; i < MIN(modules->module_count, MAX_MODULE_COUNT); i++)
    {
        target->modules.module[i].addr = mmap_io_to_phys(modules->modules[i].begin);
        target->modules.module[i].size = ALIGN_UP(modules->modules[i].end - modules->modules[i].begin, MEM_PAGE_SIZE);

        target->modules.module[i].module_name = str_cast_fix(StrFix128, modules->modules[i].string);
    }
}

void fill_handover_cmdline(Handover *target, struct stivale2_struct_tag_cmdline const *cmdline)
{
    target->cmd_lines.present = true;

    mem_cpy(target->cmd_lines.cmd_line.buffer, (void *)cmdline->cmdline, cstr_len((char *)cmdline->cmdline));
    target->cmd_lines.cmd_line.len = cstr_len((char *)cmdline->cmdline);
}

void stivale2_entry(struct stivale2_struct const *info)
{
    static Handover handover = {};

    handover.tag = HANDOVER_TAG;
    handover.boolloader_from = HANDOVER_BOOT_SRC_STIVALE2;

    auto memory_map = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    if (memory_map)
    {
        fill_handover_mmap(&handover, (struct stivale2_struct_tag_memmap const *)memory_map);
    }

    auto rsdp = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_RSDP_ID);

    if (rsdp)
    {
        fill_handover_rsdp(&handover, (struct stivale2_struct_tag_rsdp const *)rsdp);
    }

    auto framebuffer = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    if (framebuffer)
    {
        fill_handover_framebuffer(&handover, (struct stivale2_struct_tag_framebuffer const *)framebuffer);
    }

    auto modules = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MODULES_ID);

    if (modules)
    {
        fill_handover_modules(&handover, (struct stivale2_struct_tag_modules const *)modules);
    }

    auto cmd_line = stivale2_get_tag(info, STIVALE2_STRUCT_TAG_CMDLINE_ID);

    if (cmd_line)
    {
        fill_handover_cmdline(&handover, (struct stivale2_struct_tag_cmdline const *)cmd_line);
    }

    arch_entry_main(&handover);

    for (;;)
    {
        asm_cli();
        asm_hlt();
    }
}
