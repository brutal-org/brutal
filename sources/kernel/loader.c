
#include <arch/vmm.h>
#include <brutal/alloc.h>
#include <kernel/loader.h>

typedef result_t(br_error_t, monostate_t) programm_load_result_t;

static bool is_valid_elf(const struct elf64_header *header, size_t data_size)
{

    if (data_size < sizeof(struct elf64_header))
    {
        return false;
    }

    if (header->identifier.magics[0] != '\177' || header->identifier.magics[1] != 'E' || header->identifier.magics[2] != 'L' || header->identifier.magics[3] != 'F')
    {
        return false;
    }

    // don't support 32bit for the moment
    if (header->identifier.elf_class != ELF_CLASS_64)
    {
        return false;
    }

    // only support little endian
    if (header->identifier.data_encoding != ELF_ENCODING_LITTLE_ENDIAN)
    {
        return false;
    }

    // don't start non executable files
    if (header->object_type != ELF_TYPE_EXECUTABLE)
    {
        return false;
    }
    return true;
}

static programm_load_result_t map_program_header_load(struct task *task, const struct elf64_program_header *header, void *data)
{
    const size_t max_size = MAX(header->memory_size, header->file_size);

    uint8_t *segment_copy = alloc_make_array(alloc_global(), uint8_t, max_size);

    pmm_range_t phys_range = TRY(programm_load_result_t, vmm_virt2phys(vmm_kernel_space(), (vmm_range_t){
                                                                                               .size = max_size,
                                                                                               .base = (uintptr_t)segment_copy}));

    mem_set(segment_copy, 0, max_size);
    mem_cpy(segment_copy, data + header->file_offset, header->file_size);

    TRY(programm_load_result_t, vmm_map(task->virtual_memory_space, (vmm_range_t){
                                            .base = header->virtual_address,
                                            .size = header->memory_size,
                                        },
            phys_range, BR_MEM_USER | BR_MEM_WRITABLE));

    return OK(programm_load_result_t, 0);
}

static programm_load_result_t map_program_header_entry(struct task *task, const struct elf64_program_header *header, void *data)
{
    if (header->type == ELF_PROGRAM_HEADER_LOAD)
    {
        TRY(programm_load_result_t, map_program_header_load(task, header, data));
        return OK(programm_load_result_t, 0);
    }
    else if (header->type == ELF_PROGRAM_HEADER_NULL)
    {
        return OK(programm_load_result_t, 0);
    }
    else
    {
        return ERR(programm_load_result_t, 0);
    }
}

static programm_load_result_t init_program_memory_space(struct task *task, struct elf64_header *header, void *data)
{
    struct elf64_program_header *program_header_entry = (struct elf64_program_header *)(data + header->program_header_table_file_offset);

    for (size_t i = 0; i < header->program_header_table_entry_count; i++)
    {
        TRY(programm_load_result_t, map_program_header_entry(task, program_header_entry, data));
        program_header_entry = (struct elf64_program_header *)((uintptr_t)program_header_entry + header->program_header_table_entry_size);
    }
    return OK(programm_load_result_t, 0);
}

task_return_result_t program_load(str_t name, void *data, size_t size)
{
    struct elf64_header *elf = (struct elf64_header *)data;

    if (!is_valid_elf(elf, size))
    {
        return ERR(task_return_result_t, 0);
    }

    struct task *task = TRY(task_return_result_t, task_create(name, TASK_USER));

    TRY(task_return_result_t, init_program_memory_space(task, elf, data));

    return OK(task_return_result_t, task);
}
