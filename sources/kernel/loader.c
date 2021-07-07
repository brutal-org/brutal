
#include <arch/vmm.h>
#include <brutal/alloc.h>
#include <brutal/log.h>
#include <kernel/loader.h>
#include "arch/heap.h"
#include "arch/pmm.h"
#include "brutal/base/types.h"
#include "syscalls/error.h"
typedef Result(BrError, MonoState) programm_load_Result;

static bool is_valid_elf(const struct elf64_header *header, size_t data_size)
{

    if (data_size < sizeof(struct elf64_header))
    {
        return false;
    }

    if (header->identifier.magics[0] != '\177' || header->identifier.magics[1] != 'E' || header->identifier.magics[2] != 'L' || header->identifier.magics[3] != 'F')
    {
        log("not magic ");
        return false;
    }

    // don't support 32bit for the moment
    if (header->identifier.elf_class != ELF_CLASS_64)
    {
        log("not 64bit ");
        return false;
    }

    // only support little endian
    if (header->identifier.data_encoding != ELF_ENCODING_LITTLE_ENDIAN)
    {
        log("not little endian");
        return false;
    }

    // don't start non executable files
    return true;
}

static programm_load_Result map_program_header_load(struct task *task, const struct elf64_program_header *header, void *data)
{
    const size_t max_size = MAX(header->memory_size, header->file_size);

    uint8_t *segment_copy = (uint8_t *)TRY(programm_load_Result, heap_alloc(max_size)).base;

    PmmRange phys_range = TRY(programm_load_Result, vmm_virt2phys(vmm_kernel_space(), (VmmRange){
                                                                                          .size = ALIGN_UP(header->memory_size, HOST_MEM_PAGESIZE),
                                                                                          .base = (uintptr_t)segment_copy}));

    log("map_program_header_load");
    VmmRange vmm_range = (VmmRange){
        .base = header->virtual_address,
        .size = ALIGN_UP(header->memory_size, HOST_MEM_PAGESIZE),
    };

    log("vmm range: {#p}:{#p}, phys range: {#p}:{#p}", vmm_range.base, vmm_range.size, phys_range.base, phys_range.size);

    mem_set(segment_copy, 0, max_size);
    mem_cpy(segment_copy, data + header->file_offset, header->file_size);

    TRY(programm_load_Result, vmm_map(task->virtual_memory_space, vmm_range,
                                      phys_range, BR_MEM_USER | BR_MEM_WRITABLE));

    return OK(programm_load_Result, 0);
}

static programm_load_Result map_program_header_entry(struct task *task, const struct elf64_program_header *header, void *data)
{
    if (header->type == ELF_PROGRAM_HEADER_LOAD)
    {
        TRY(programm_load_Result, map_program_header_load(task, header, data));
        return OK(programm_load_Result, 0);
    }
    else if (header->type == ELF_PROGRAM_HEADER_NULL || header->type == ELF_PROGRAM_HEADER_NOTE)
    {
        return OK(programm_load_Result, 0);
    }
    else
    {
        log("programm invalid header type: {}", header->type);
        return OK(programm_load_Result, 0);
    }
}

static programm_load_Result init_program_memory_space(struct task *task, struct elf64_header *header, void *data)
{
    struct elf64_program_header *program_header_entry = (struct elf64_program_header *)(data + header->program_header_table_file_offset);

    for (size_t i = 0; i < header->program_header_table_entry_count; i++)
    {
        TRY(programm_load_Result, map_program_header_entry(task, program_header_entry, data));
        program_header_entry = (struct elf64_program_header *)((uintptr_t)program_header_entry + header->program_header_table_entry_size);
    }
    return OK(programm_load_Result, 0);
}

task_return_Result program_load(Str name, void *data, size_t size, uintptr_t *start_addr)
{
    struct elf64_header *elf = (struct elf64_header *)data;

    if (!is_valid_elf(elf, size))
    {
        log("invalid elf");
        return ERR(task_return_Result, 0);
    }

    struct task *task = TRY(task_return_Result, task_create(name, TASK_USER));

    TRY(task_return_Result, init_program_memory_space(task, elf, data));

    *start_addr = elf->entry;

    return OK(task_return_Result, task);
}
