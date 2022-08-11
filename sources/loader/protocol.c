#include <bal/abi.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-io>
#include <efi/lib.h>
#include <efi/srvs.h>
#include <elf/elf.h>

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

void efi_populate_mmap(HandoverMmap *mmap)
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

EfiStatus efi_lookup_acpi(EfiGuid table_guid, void **table)
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

uintptr_t efi_find_rsdp(void)
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

int eif_gop_find_mode(EFIGraphicsOutputProtocol *gop, size_t req_width, size_t req_height)
{
    EFIGraphicsOutputModeInfo *info;
    for (uint32_t i = 0; i < gop->mode->max_mode; i++)
    {
        uint64_t info_size = 0;
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
        uint64_t info_size = 0;
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

void efi_populate_framebuffer(EFIBootServices *bs, HandoverFramebuffer *fb, LoaderFramebuffer const *req_fb)
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

void efi_load_module(HandoverModule *target, Str path)
{
    IoFile file;
    io_file_view(&file, path);

    size_t module_size = UNWRAP(io_size(io_file_seeker(&file)));
    uintptr_t module_addr = kernel_module_phys_alloc_page(align_up$(module_size, PAGE_SIZE) / PAGE_SIZE);

    UNWRAP(io_read(io_file_reader(&file), (void *)module_addr, module_size));

    target->addr = module_addr;
    target->size = align_up$(module_size, PAGE_SIZE);

    log$("Loaded module data '{}' (begin: {p} end: {p})...", path, target->addr, target->addr + target->size);
}

void efi_load_modules(LoaderEntry const *entry, HandoverModules *modules)
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

void efi_load_elf(Elf64Header const *elf_header, void *base, VmmSpace vmm)
{
    Elf64ProgramHeader *prog_header = (Elf64ProgramHeader *)(base + elf_header->programs_offset);

    for (int i = 0; i < elf_header->programs_count; i++)
    {
        if (prog_header->type == ELF_PROGRAM_HEADER_LOAD)
        {
            void *file_segment = (void *)((uint64_t)base + prog_header->file_offset);

            void *mem_phys_segment = (void *)kernel_module_phys_alloc_page_addr(align_up$(prog_header->memory_size, PAGE_SIZE) / PAGE_SIZE, prog_header->virtual_address - MMAP_KERNEL_BASE);

            mem_cpy(mem_phys_segment, file_segment, prog_header->file_size);
            mem_set(mem_phys_segment + prog_header->file_size, 0, prog_header->memory_size - prog_header->file_size);
            memory_map_range(
                vmm,
                (VmmRange){
                    .base = prog_header->virtual_address,
                    .size = align_up$(prog_header->memory_size, PAGE_SIZE),
                },
                (PmmRange){
                    .base = (uintptr_t)mem_phys_segment,
                    .size = align_up$(prog_header->memory_size, PAGE_SIZE),
                });
        }
        else
        {
            log$("Unkown program header: {}", prog_header->type);
        }

        prog_header = (Elf64ProgramHeader *)((void *)prog_header + elf_header->programs_size);
    }
}

EntryPointFn *efi_load_kernel(Str path, VmmSpace vmm)
{
    log$("Loading elf file...");

    IoFile file;
    io_file_view(&file, path);

    Buf buf;
    io_read_all(io_file_reader(&file), &buf, alloc_global());

    log$("Loaded elf file...");
    Elf64Header *header = (Elf64Header *)buf.data;

    if (buf.used < sizeof(Elf64Header) ||
        !elf_validate(header))
    {
        panic$("Invalid elf file!");
    }

    log$("Elf file loaded in memory, mapping it...");

    efi_load_elf(header, buf.data, vmm);

    uintptr_t entry = header->entry;

    buf_deinit(&buf);

    log$("Entry is {#x}", entry);

    return (EntryPointFn *)entry;
}

EntryPointFn *efi_populate_handover(LoaderEntry const *entry, Handover *ho, VmmSpace vmm)
{
    ho->tag = HANDOVER_TAG;
    ho->rsdp = efi_find_rsdp();

    log$("Loading kernel...");
    EntryPointFn *entry_point = efi_load_kernel(entry->kernel, vmm);

    log$("Loading modules...");
    efi_load_modules(entry, &ho->modules);

    log$("Loading framebuffer...");
    efi_populate_framebuffer(efi_st()->boot_services, &ho->framebuffer, &entry->framebuffer);

    log$("Populating mmap...");
    efi_populate_mmap(&ho->mmap);

    return entry_point;
}
