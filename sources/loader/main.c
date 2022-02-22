#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <elf/elf.h>
#include "config.h"
#include "loader.h"
#include "memory.h"
#include "protocol.h"

typedef void (*EntryPointFn)(Handover *handover, uint64_t t) __attribute__((sysv_abi));

void __chkstk() { return; }

void loader_splash(void)
{
    print(io_chan_out(), "BRUTAL boot\n");
}

void loader_load(Elf64Header const *elf_header, void *base, VmmSpace vmm)
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
            memory_map_range(vmm, (VmmRange){
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

EntryPointFn loader_load_kernel(Str path, VmmSpace vmm)
{
    IoFile file;
    io_file_open(&file, path);
    Buf buf;
    io_read_all(io_file_reader(&file), &buf, alloc_global());

    Elf64Header *header = (Elf64Header *)buf.data;

    if (buf.used < sizeof(Elf64Header) ||
        !elf_validate(header))
    {
        panic$("Invalid elf file!");
    }

    loader_load(header, buf.data, vmm);

    uintptr_t entry = header->entry;

    buf_deinit(&buf);

    return (EntryPointFn)entry;
}

Handover *allocate_handover(VmmSpace vmm)
{
    uintptr_t handover_copy_phys = kernel_module_phys_alloc_page((sizeof(Handover) / PAGE_SIZE) + 1);

    memory_map_range(vmm, (VmmRange){
                              .base = handover_copy_phys + MMAP_KERNEL_BASE,
                              .size = align_up$(sizeof(Handover), PAGE_SIZE),
                          },
                     (PmmRange){
                         .base = handover_copy_phys,
                         .size = align_up$(sizeof(Handover), PAGE_SIZE),
                     });

    return (Handover *)(handover_copy_phys);
}

void loader_boot(LoaderEntry const *entry)
{
    VmmSpace vmm = memory_create();

    EntryPointFn entry_point = loader_load_kernel(entry->kernel, vmm);

    Handover *handover = allocate_handover(vmm);

    loader_populate_handover(entry, handover);

    memory_switch(vmm);

    loader_boot_deinit();

    entry_point(((void *)handover) + MMAP_KERNEL_BASE, 0xC001B001);

    panic$("kernel shouldn't return!");
}

void loader_entry(void)
{
    loader_splash();

    LoaderEntry entry = config_get_entry(str$("Brutal"), str$("/boot/config.json"));

    loader_boot(&entry);
}
