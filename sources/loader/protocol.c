#include <bal/abi.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <efi/lib.h>
#include <efi/srvs.h>
#include "loader/protocol.h"

static HandoverMmapType efi_mmap_type_to_handover[] = {
    [EFI_RESERVED_MEMORY_TYPE] = HANDOVER_MMAP_RESERVED,
    [EFI_RUNTIME_SERVICES_CODE] = HANDOVER_MMAP_RESERVED,
    [EFI_RUNTIME_SERVICES_DATA] = HANDOVER_MMAP_RESERVED,
    [EFI_MEMORY_MAPPED_IO] = HANDOVER_MMAP_RESERVED,
    [EFI_MEMORY_MAPPED_IO_PORT_SPACE] = HANDOVER_MMAP_RESERVED,
    [EFI_PAL_CODE] = HANDOVER_MMAP_RESERVED,
    [EFI_UNUSABLE_MEMORY] = HANDOVER_MMAP_RESERVED,
    [EFI_ACPI_RECLAIM_MEMORY] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_LOADER_CODE] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_LOADER_DATA] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_BOOT_SERVICES_CODE] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_BOOT_SERVICES_DATA] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_CONVENTIONAL_MEMORY] = HANDOVER_MMAP_FREE,
    [EFI_ACPI_MEMORY_NVS] = HANDOVER_MMAP_RESERVED,
};

HandoverMmap get_mmap()
{

    size_t entry_count = 0;
    size_t descriptor_size = 0;
    EFIMemoryDescriptor *mmap = get_efi_memory_map(&entry_count, &descriptor_size);

    HandoverMmap *h_mmap = alloc_malloc(alloc_global(), (sizeof(HandoverMmap) + (entry_count) * sizeof(HandoverMmapEntry)));

    mem_set(h_mmap, 0, sizeof(HandoverMmap) + (entry_count) * sizeof(HandoverMmapEntry));

    HandoverMmapEntry *start_from = h_mmap->entries;

    int last_type = -1;
    uint64_t last_end = 0xFFFFFFFFFFFF;

    uint64_t entries = 0;
    for (uint64_t i = 0; i < entry_count; i++)
    {
        EFIMemoryDescriptor *desc = (EFIMemoryDescriptor *)((uint64_t)mmap + descriptor_size * i);

        uint64_t length = desc->num_pages << 12;
        uint64_t phys_base = desc->physical_start;
        uint64_t phys_end = desc->physical_start + length;

        int type = HANDOVER_MMAP_RESERVED;

        if (desc->type < EFI_MAX_MEMORY_TYPE)
        {
            type = efi_mmap_type_to_handover[desc->type];
        }

        if (last_type == type && last_end == desc->physical_start)
        {
            start_from[-1].length += length;
        }

        else
        {
            start_from->type = type;
            start_from->length = length;
            start_from->base = phys_base;
            last_type = type;

            start_from++;
            entries++;
        }

        last_end = phys_end;
    }

    h_mmap->size = entries;

    return *h_mmap;
}

EFIStatus get_system_config_table(EFIGUID table_guid, void **table)
{
    for (uint64_t i = 0; i < efi_st()->num_table_entries; i++)
    {
        if (efi_guid_eq(table_guid, efi_st()->config_table[i].vendor_guid))
        {
            *table = efi_st()->config_table[i].vendor_table;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}

uintptr_t get_rsdp()
{
    void *acpi_table = nullptr;

    EFIGUID guid = ACPI_TABLE_GUID;
    EFIGUID guid2 = ACPI2_TABLE_GUID;

    if (get_system_config_table(guid, &acpi_table) == EFI_SUCCESS ||
        get_system_config_table(guid2, &acpi_table) == EFI_SUCCESS)
    {
        return (uintptr_t)acpi_table;
    }
    else
    {
        return 0;
    }
}

HandoverFramebuffer get_framebuffer(EFIBootServices *bs)
{
    EFIGUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFIGraphicsOutputProtocol *gop;

    bs->locate_protocol(&gop_guid, nullptr, (void **)&gop);

    EFIGraphicsOutputModeInfo *info;

    uint64_t size_of_info;

    EFIStatus status = gop->query_mode(gop, gop->mode == nullptr ? 0 : gop->mode->mode, &size_of_info, &info);

    if (status == EFI_NOT_STARTED)
    {
        status = gop->set_mode(gop, 0);
    }

    if (status != EFI_SUCCESS)
    {
        panic$("Couldn't get framebuffer native mode");
    }

    uint64_t addr = gop->mode->framebuffer_base;
    uint32_t width = gop->mode->info->horizontal_resolution;
    uint32_t height = gop->mode->info->vertical_resolution;
    uint32_t pitch = gop->mode->info->pixels_per_scan_line * gop->mode->info->pixel_format;

    return (HandoverFramebuffer){true, addr, width, height, pitch, gop->mode->info->pixel_format};
}

Handover get_handover(void)
{
    EFIBootServices *bs = efi_st()->boot_services;
    HandoverFramebuffer fb = get_framebuffer(bs);
    uintptr_t rsdp = get_rsdp();
    HandoverMmap mmap = get_mmap();

    return (Handover){
        .framebuffer = fb,
        .rsdp = rsdp,
        .mmap = mmap,
    };
}
