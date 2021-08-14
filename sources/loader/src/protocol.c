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

    EFIStatus status = EFI_SUCCESS;
    u64 mem_map_size = sizeof(EFIMemoryDescriptor) * 16;
    u64 mem_map_key = 0;
    u64 mem_map_desc_size = 0;
    u32 mem_map_desc_ver = 0;
    u64 desc_count = 0;
    uint8_t *buffer = NULL;
    EFIMemoryDescriptor *entry_ptr = NULL;

    do
    {
        bs->allocate_pool(EFI_LOADER_DATA, mem_map_size, (void **)&buffer);

        if (buffer == NULL)
            break;

        status = bs->get_memory_map(&mem_map_size, (EFIMemoryDescriptor *)buffer, &mem_map_key, &mem_map_desc_size, &mem_map_desc_ver);

        if (status != EFI_SUCCESS)
        {
            bs->free_pool(buffer);
            mem_map_size += 2 * mem_map_desc_size;
        }
    } while (status != EFI_SUCCESS);

    desc_count = mem_map_size / mem_map_desc_size;

    entry_ptr = (EFIMemoryDescriptor *)buffer;

    u64 pages_count = 0;

    u64 greatest_pages = 0;
    u64 greatest_type = 0;

    for (u64 i = 0; i < desc_count; i++)
    {
        entry_ptr = (EFIMemoryDescriptor *)(buffer + (i * mem_map_desc_size));

        if (entry_ptr->num_pages > greatest_pages)
        {

            greatest_pages = entry_ptr->num_pages;
            greatest_type = entry_ptr->type;

            efi_print("new greatest #{} {} {}", i, greatest_type, greatest_pages);
        }
        pages_count += entry_ptr->num_pages;
    }

    bs->free_pool(buffer);

    efi_print("{} {}", greatest_type, greatest_pages);
    efi_print("{}", pages_count * 4096 / 1024 / 1024);
    // u64 count = map_size / desc_size;

    bs->exit_boot_services(image_handle, mem_map_key);

    return (HandoverMmap){0, {0}};
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

    UNUSED(image_handle);
    
    return (Handover){
        .framebuffer = fb, .rsdp = rsdp};
}
