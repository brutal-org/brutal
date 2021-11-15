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

HandoverMmap get_mmap(void)
{
    size_t entry_count = 0;
    size_t descriptor_size = 0;
    EFIMemoryDescriptor *mmap = get_efi_memory_map(&entry_count, &descriptor_size);

    HandoverMmap *h_mmap = alloc_malloc(alloc_global(), (sizeof(HandoverMmap) + (entry_count) * sizeof(HandoverMmapEntry)));

    mem_set(h_mmap, 0, sizeof(HandoverMmap) + (entry_count) * sizeof(HandoverMmapEntry));

    HandoverMmapEntry *current = h_mmap->entries;
    HandoverMmapEntry *previous_entry = nullptr;
    uint64_t final_entry_count = 0;

    for (uint64_t i = 0; i < entry_count; i++)
    {
        EFIMemoryDescriptor *desc = (EFIMemoryDescriptor *)((uint64_t)mmap + descriptor_size * i);

        uint64_t len = desc->num_pages << 12;
        uint64_t phys_base = desc->physical_start;

        HandoverMmapType type;

        if (desc->type == UEFI_MEM_BRUTAL_KERNEL_MODULE)
        {
            type = HANDOVER_MMAP_KERNEL_MODULE;
        }
        else
        {
            type = efi_mmap_type_to_handover[desc->type];
        }

        if (previous_entry != nullptr &&
            previous_entry->type == type &&
            (previous_entry->base + previous_entry->len) == desc->physical_start)
        {
            previous_entry->len += len;
        }
        else
        {
            current->type = type;
            current->len = len;
            current->base = phys_base;

            previous_entry = current;

            current++;
            final_entry_count++;
        }
    }

    h_mmap->size = final_entry_count;

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

static int get_gop_mode(EFIGraphicsOutputProtocol *gop, size_t req_width, size_t req_height)
{
    EFIGraphicsOutputModeInfo *info;
    for (uint32_t i = 0; i < gop->mode->max_mode; i++)
    {
        size_t info_size = 0;
        EFIStatus status = gop->query_mode(gop, i, &info_size, &info);
        if (status == EFI_ERR)
        {
            log$("can't get info for: {}", i);
        }

        if (info->vertical_resolution == req_height && info->horizontal_resolution == req_width)
        {
            return i;
        }
    }

    log$("can't find efi gop mode for: {}x{}", req_width, req_height);
    log$("available mode: ");
    for (uint32_t i = 0; i < gop->mode->max_mode; i++)
    {
        size_t info_size = 0;
        EFIStatus status = gop->query_mode(gop, i, &info_size, &info);
        if (status == EFI_ERR)
        {
            log$(" - can't get info for: {}", i);
        }
        else
        {
            log$(" - {} = {}x{}", i, info->horizontal_resolution, info->vertical_resolution);
        }
    }

    return 0;
}

static HandoverFramebuffer get_framebuffer(EFIBootServices *bs, LoaderFramebuffer const *fb)
{
    EFIGraphicsOutputProtocol *gop;
    EFIStatus status;

    EFIGUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    bs->locate_protocol(&gop_guid, nullptr, (void **)&gop);
    int mode = get_gop_mode(gop, fb->width, fb->height);
    status = gop->set_mode(gop, mode);

    if (status != EFI_SUCCESS)
    {
        panic$("Couldn't get framebuffer native mode");
    }

    EFIGraphicsOutputModeInfo *gop_info = gop->mode->info;

    assert_truth(gop_info->pixel_format == PIXEL_BGR_8_BIT);

    return (HandoverFramebuffer){
        .present = true,
        .addr = gop->mode->framebuffer_base,
        .width = gop_info->horizontal_resolution,
        .height = gop_info->vertical_resolution,
        .pitch = gop_info->pixels_per_scan_line * sizeof(uint32_t),
        .bpp = 32,
    };
}

static void load_module_data(HandoverModule *target, Str path)
{
    IoFile file;
    IoReader reader;
    Buf buf;

    io_file_open(&file, path);
    reader = io_file_reader(&file);

    // TODO: make a kernel_module alloc instead of allocating 2 time (even if the first time is always freed)
    buf = io_readall((&reader), alloc_global());

    uintptr_t buf_page_size = ALIGN_UP(buf.used, PAGE_SIZE) / PAGE_SIZE;
    uintptr_t module_addr = kernel_module_phys_alloc_page(buf_page_size);

    memcpy((void *)module_addr, buf.data, buf.used);

    target->size = ALIGN_UP(buf.used, PAGE_SIZE);
    target->addr = module_addr;

    buf_deinit(&buf);

    log$("Loading module data '{}' (size: {})...", path, target->size);
}

static HandoverModules get_handover_modules(LoaderEntry const *entry)
{
    HandoverModules res = {};
    int id = 0;
    vec_foreach(module, &entry->modules)
    {
        HandoverModule *target = &res.module[id];

        memcpy(target->module_name.buf, module.name.buf, module.name.len);
        target->module_name.len = module.name.len;

        load_module_data(target, module.path);

        id++;
    }

    res.size = id;

    return res;
}

Handover get_handover(LoaderEntry const *entry)
{
    HandoverFramebuffer fb = get_framebuffer(efi_st()->boot_services, &entry->framebuffer);
    HandoverModules modules = get_handover_modules(entry);
    uintptr_t rsdp = get_rsdp();
    HandoverMmap mmap = get_mmap();

    return (Handover){
        .framebuffer = fb,
        .rsdp = rsdp,
        .mmap = mmap,
        .modules = modules,
    };
}
