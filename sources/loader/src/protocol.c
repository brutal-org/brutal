#include "protocol.h"
#include <brutal/log.h>
#include <efi/utils.h>
#include "efi/base.h"
#include "efi/bs.h"
#include "efi/st.h"
#include "handover/handover.h"

MAYBE_UNUSED static HandoverMmapType efi_mmap_type_to_handover[] = {
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
    [EFI_ACPI_MEMORY_NVS] = HANDOVER_MMAP_RESERVED};

void query_gop_modes(EFIGraphicsOutputProtocol *gop, u64 num_modes, u64 native_mode)
{

    EFIGraphicsOutputModeInfo *info;
    u64 size_of_info;

    for (u64 i = 0; i < num_modes; i++)
    {
        gop->query_mode(gop, i, &size_of_info, &info);
        efi_print("mode {} width {} height {} format {}{}", i, info->horizontal_resolution, info->vertical_resolution, info->pixel_format, i == native_mode ? "(current)" : "");
    }
}

HandoverMmap get_mmap(EFIBootServices *bs, MAYBE_UNUSED EFIHandle *image_handle)
{

    u8 tmp_mmap[1];
    u64 mmap_size = sizeof(tmp_mmap);
    u64 map_key = 0;
    u64 descriptor_size = 0;
    u32 descriptor_version = 0;

    bs->get_memory_map(&mmap_size, (EFIMemoryDescriptor *)tmp_mmap, &map_key, &descriptor_size, &descriptor_version);

    mmap_size += 4096;

    EFIMemoryDescriptor *mmap = alloc_malloc(alloc_global(), mmap_size);

    bs->get_memory_map(&mmap_size, mmap, &map_key, &descriptor_size, &descriptor_version);

    u64 entry_count = (mmap_size / descriptor_size);

    HandoverMmap *h_mmap = alloc_malloc(alloc_global(), (sizeof(HandoverMmap) + (mmap_size / descriptor_size) * sizeof(HandoverMmapEntry)));

    mem_set(h_mmap, 0, sizeof(HandoverMmap) + (mmap_size / descriptor_size) * sizeof(HandoverMmapEntry));

    HandoverMmapEntry *start_from = h_mmap->entries;

    u64 pages_count = 0;

    int last_type = -1;
    u64 last_end = 0xFFFFFFFFFFFF;

    u64 entries = 0;
    for (u64 i = 0; i < entry_count; i++)
    {
        EFIMemoryDescriptor *desc = (EFIMemoryDescriptor *)((u64)mmap + descriptor_size * i);

        u64 length = desc->num_pages << 12;
        u64 phys_base = desc->physical_start;
        u64 phys_end = desc->physical_start + length;

        int type = HANDOVER_MMAP_RESERVED;

        if (desc->type < EFI_MAX_MEMORY_TYPE)
        {
            type = efi_mmap_type_to_handover[desc->type];
        }

        pages_count += length;

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

    
    bs->exit_boot_services(image_handle, map_key);
    
    return *h_mmap;
}

uintptr_t get_rsdp()
{
    void *acpi_table = NULL;
    EFIGUID guid2 = ACPI2_TABLE_GUID;
    EFIGUID guid = ACPI_TABLE_GUID;

    EFIStatus status;

    if ((status = get_system_config_table(&guid2, &acpi_table)) == EFI_SUCCESS)
    {
        return (uintptr_t)acpi_table;
    }

    else if ((status = get_system_config_table(&guid, &acpi_table)) == EFI_SUCCESS)
    {
        return (uintptr_t)acpi_table;
    }

    return 0;
}

HandoverFramebuffer get_framebuffer(EFIBootServices *bs)
{
    EFIGUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFIGraphicsOutputProtocol *gop;

    bs->locate_protocol(&gop_guid, NULL, (void **)&gop);

    EFIGraphicsOutputModeInfo *info;

    u64 size_of_info, num_modes, native_mode;

    auto status = gop->query_mode(gop, gop->mode == NULL ? 0 : gop->mode->mode, &size_of_info, &info);

    if (status == EFI_NOT_STARTED)
    {
        status = gop->set_mode(gop, 0);
    }

    if (status != EFI_SUCCESS)
    {
        panic("Couldn't get framebuffer native mode");
    }

    else
    {
        native_mode = gop->mode->mode;
        num_modes = gop->mode->max_mode;
    }

    auto addr = gop->mode->framebuffer_base;
    auto width = gop->mode->info->horizontal_resolution;
    auto height = gop->mode->info->vertical_resolution;
    auto pitch = gop->mode->info->pixels_per_scan_line * gop->mode->info->pixel_format;

    return (HandoverFramebuffer){true, addr, width, height, pitch, gop->mode->info->pixel_format};
}

Handover get_handover(EFISystemTable *st, EFIHandle *image_handle)
{
    auto bs = st->boot_services;
    auto fb = get_framebuffer(bs);
    auto rsdp = get_rsdp();
    MAYBE_UNUSED auto mmap = get_mmap(bs, image_handle);

    return (Handover){
        .framebuffer = fb, .rsdp = rsdp, .mmap = mmap};
}
