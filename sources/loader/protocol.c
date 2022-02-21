#include <bal/abi.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <efi/lib.h>
#include <efi/srvs.h>
#include "loader/memory.h"
#include "loader/protocol.h"

static HandoverMmapType efi_mmap_type_to_handover[] = {
    [EFI_RESERVED_MEMORY_TYPE] = HANDOVER_MMAP_RESERVED,
    [EFI_LOADER_CODE] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_LOADER_DATA] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_BOOT_SERVICES_CODE] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_BOOT_SERVICES_DATA] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_RUNTIME_SERVICES_CODE] = HANDOVER_MMAP_RESERVED,
    [EFI_RUNTIME_SERVICES_DATA] = HANDOVER_MMAP_RESERVED,
    [EFI_CONVENTIONAL_MEMORY] = HANDOVER_MMAP_FREE,
    [EFI_UNUSABLE_MEMORY] = HANDOVER_MMAP_RESERVED,
    [EFI_ACPI_RECLAIM_MEMORY] = HANDOVER_MMAP_RECLAIMABLE,
    [EFI_ACPI_MEMORY_NVS] = HANDOVER_MMAP_RESERVED,
    [EFI_MEMORY_MAPPED_IO] = HANDOVER_MMAP_RESERVED,
    [EFI_MEMORY_MAPPED_IO_PORT_SPACE] = HANDOVER_MMAP_RESERVED,
    [EFI_PAL_CODE] = HANDOVER_MMAP_RESERVED,
    [EFI_PERSISTENT_MEMORY] = HANDOVER_MMAP_RESERVED,
};

static void efi_populate_mmap(HandoverMmap *mmap)
{
    mmap->size = 0;

    size_t desc_count = 0;
    size_t desc_size = 0;
    EFIMemoryDescriptor *desc = efi_mmap_snapshot(&desc_count, &desc_size);

    for (size_t i = 0; i < desc_count; i++)
    {
        desc = (EFIMemoryDescriptor *)((void *)desc + desc_size);

        uint64_t base = desc->physical_start;
        uint64_t size = desc->num_pages << 12;
        HandoverMmapType type = HANDOVER_MMAP_USED;

        if (desc->type != EFI_USER_KERNEL_MEMORY)
        {
            type = efi_mmap_type_to_handover[desc->type];
        }

        handover_mmap_append(
            mmap,
            (HandoverMmapEntry){
                .base = base,
                .size = size,
                .type = type,
            });
    }
}

static EfiStatus efi_lookup_acpi(EfiGuid table_guid, void **table)
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

static uintptr_t efi_find_rsdp(void)
{
    void *acpi_table = nullptr;

    EfiGuid guid = ACPI_TABLE_GUID;
    EfiGuid guid2 = ACPI2_TABLE_GUID;

    if (efi_lookup_acpi(guid, &acpi_table) == EFI_SUCCESS ||
        efi_lookup_acpi(guid2, &acpi_table) == EFI_SUCCESS)
    {
        return (uintptr_t)acpi_table;
    }
    else
    {
        return 0;
    }
}

static int eif_gop_find_mode(EFIGraphicsOutputProtocol *gop, size_t req_width, size_t req_height)
{
    EFIGraphicsOutputModeInfo *info;
    for (uint32_t i = 0; i < gop->mode->max_mode; i++)
    {
        size_t info_size = 0;
        EfiStatus status = gop->query_mode(gop, i, &info_size, &info);
        if (status == EFI_ERR)
        {
            log$("Can't get info for: {}", i);
        }

        if (info->vertical_resolution == req_height && info->horizontal_resolution == req_width &&
            info->pixel_format == PIXEL_BGR_8_BIT)
        {
            return i;
        }
    }

    log$("Can't find efi gop mode for: {}x{}", req_width, req_height);
    log$("Available mode: ");
    for (uint32_t i = 0; i < gop->mode->max_mode; i++)
    {
        size_t info_size = 0;
        EfiStatus status = gop->query_mode(gop, i, &info_size, &info);
        if (status == EFI_ERR)
        {
            log$(" - can't get info for: {}", i);
        }
        else
        {
            log$(" - {} = {}x{}", i, info->horizontal_resolution, info->vertical_resolution);
        }
    }

    return gop->mode->max_mode - 1;
}

static void efi_populate_framebuffer(EFIBootServices *bs, HandoverFramebuffer *fb, LoaderFramebuffer const *req_fb)
{
    EFIGraphicsOutputProtocol *gop;
    EfiGuid gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    bs->locate_protocol(&gop_guid, nullptr, (void **)&gop);

    int mode = eif_gop_find_mode(gop, req_fb->width, req_fb->height);
    EfiStatus status = gop->set_mode(gop, mode);

    if (status != EFI_SUCCESS)
    {
        panic$("Couldn't get framebuffer native mode");
    }

    EFIGraphicsOutputModeInfo *gop_info = gop->mode->info;

    assert_truth(gop_info->pixel_format == PIXEL_BGR_8_BIT);

    fb->present = true;
    fb->addr = gop->mode->framebuffer_base;
    fb->width = gop_info->horizontal_resolution;
    fb->height = gop_info->vertical_resolution;
    fb->pitch = gop_info->pixels_per_scan_line * sizeof(uint32_t);
    fb->bpp = 32;
}

static void efi_load_module(HandoverModule *target, Str path)
{
    IoFile file;
    io_file_open(&file, path, FILE_OPEN_READ_ONLY);

    // TODO: make a kernel_module alloc instead of allocating 2 time (even if the first time is always freed)
    Buf buf;
    io_read_all(io_file_reader(&file), &buf, alloc_global());

    uintptr_t buf_page_size = align_up$(buf.used, PAGE_SIZE) / PAGE_SIZE;
    uintptr_t module_addr = kernel_module_phys_alloc_page(buf_page_size);

    memcpy((void *)module_addr, buf.data, buf.used);

    target->size = align_up$(buf.used, PAGE_SIZE);
    target->addr = module_addr;

    buf_deinit(&buf);

    log$("Loaded module data '{}' (size: {})...", path, target->size);
}

static void efi_load_modules(LoaderEntry const *entry, HandoverModules *modules)
{
    int id = 0;
    vec_foreach_v(module, &entry->modules)
    {
        HandoverModule *target = &modules->module[id];

        memcpy(target->module_name.buf, module.name.buf, module.name.len);
        target->module_name.len = module.name.len;

        efi_load_module(target, module.path);

        id++;
    }

    modules->size = id;
}

void efi_populate_handover(LoaderEntry const *entry, Handover *ho)
{
    ho->tag = HANDOVER_TAG;

    efi_populate_framebuffer(efi_st()->boot_services, &ho->framebuffer, &entry->framebuffer);
    efi_load_modules(entry, &ho->modules);
    efi_populate_mmap(&ho->mmap);
    ho->rsdp = efi_find_rsdp();
}
