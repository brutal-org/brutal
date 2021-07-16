
#include <arch/vmm.h>
#include <brutal/alloc.h>
#include <brutal/log.h>
#include <kernel/loader.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "brutal/base/types.h"

typedef Result(BrResult, MonoState) ProgramloadResult;

static bool is_elf_supported(const struct elf64_header *header, size_t data_size)
{
    if (data_size < sizeof(struct elf64_header))
    {
        log("not valid elf size");
        return false;
    }

    if (!has_valid_elf_header(header))
    {
        log("not valid elf header");
    }

    if (header->identifier.elf_class != ELF_CLASS_64)
    {
        log("not 64bit ");
        return false;
    }

    if (header->identifier.data_encoding != ELF_ENCODING_LITTLE_ENDIAN)
    {
        log("not little endian");
        return false;
    }

    // don't start non executable files
    return true;
}

static HeapRange elf_loader_copy_segment(const struct elf64_program_header *header, const void *data)
{
    auto size = MAX(header->memory_size, header->file_size);
    auto range = UNWRAP(heap_alloc(size));
    auto ptr = (uint8_t *)range.base;

    mem_set(ptr, 0, size);
    mem_cpy(ptr, data + header->file_offset, header->file_size);

    return range;
}

static ProgramloadResult map_program_header_load(Task *task, const struct elf64_program_header *header, const void *data)
{
    auto range = elf_loader_copy_segment(header, data);

    VmmRange target = (VmmRange){
        .base = header->virtual_address,
        .size = ALIGN_UP(header->memory_size, HOST_MEM_PAGESIZE),
    };

    TRY(ProgramloadResult, memory_space_map_pmm(task->space, target, heap_to_pmm(range)));

    return OK(ProgramloadResult, (MonoState){});
}

static ProgramloadResult map_program_header_entry(Task *task, const struct elf64_program_header *header, const void *data)
{
    if (header->type == ELF_PROGRAM_HEADER_LOAD)
    {
        TRY(ProgramloadResult, map_program_header_load(task, header, data));
        return OK(ProgramloadResult, (MonoState){});
    }
    else if (header->type == ELF_PROGRAM_HEADER_NULL || header->type == ELF_PROGRAM_HEADER_NOTE)
    {
        return OK(ProgramloadResult, (MonoState){});
    }
    else
    {
        log("programm invalid header type: {}", header->type);
        return OK(ProgramloadResult, (MonoState){});
    }
}

static ProgramloadResult init_program_memory_space(Task *task, struct elf64_header *header, void *data)
{
    struct elf64_program_header *program_header_entry = (struct elf64_program_header *)(data + header->program_header_table_file_offset);

    for (size_t i = 0; i < header->program_header_table_entry_count; i++)
    {
        TRY(ProgramloadResult, map_program_header_entry(task, program_header_entry, data));
        program_header_entry = (struct elf64_program_header *)((uintptr_t)program_header_entry + header->program_header_table_entry_size);
    }

    return OK(ProgramloadResult, (MonoState){});
}

TaskCreateResult program_load(Str name, void *data, size_t size, uintptr_t *start_addr)
{
    struct elf64_header *elf = (struct elf64_header *)data;

    if (!is_elf_supported(elf, size))
    {
        return ERR(TaskCreateResult, BR_BAD_EXE_FORMAT);
    }

    Task *task = TRY(TaskCreateResult, task_create(name, TASK_USER));

    TRY(TaskCreateResult, init_program_memory_space(task, elf, data));

    *start_addr = elf->entry;

    return OK(TaskCreateResult, task);
}
