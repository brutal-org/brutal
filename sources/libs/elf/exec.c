#include <brutal/alloc.h>
#include <brutal/log.h>
#include <elf/exec.h>
#include <syscalls/helpers.h>
#include <syscalls/syscalls.h>

void elf_load(BrSpace space, Elf64Header const *elf_header, BrMemObj elf_obj)
{
    auto prog_header = (Elf64ProgramHeader *)((uint8_t *)elf_header + elf_header->program_header_table_file_offset);

    for (size_t i = 0; i < elf_header->program_header_table_entry_count; i++)
    {
        if (prog_header->type != ELF_PROGRAM_HEADER_LOAD)
        {
            log("Ignoring segment with type={#0x}", prog_header->type);
            continue;
        }

        auto size = ALIGN_UP(MAX(prog_header->memory_size, prog_header->file_size), MEM_PAGE_SIZE);

        if (!(prog_header->flags & ELF_PROGRAM_HEADER_WRITABLE) &&
            prog_header->file_size == prog_header->memory_size)
        {
            BrMapArgs prog_map = {
                .space = space,
                .mem_obj = elf_obj,
                .offset = prog_header->file_offset,
                .size = size,
                .vaddr = prog_header->virtual_address,
                .flags = BR_MEM_NONE,
            };

            assert_truth(br_map(&prog_map) == BR_SUCCESS);
        }
        else
        {
            BrCreateArgs prog_obj = {
                .type = BR_OBJECT_MEMORY,
                .mem_obj = {
                    .size = size,
                    .flags = BR_MEM_OBJ_NONE,
                },
            };

            assert_truth(br_create(&prog_obj) == BR_SUCCESS);

            BrMapArgs prog_local_map = {
                .space = BR_SPACE_SELF,
                .mem_obj = prog_obj.handle,
                .flags = BR_MEM_WRITABLE,
            };

            assert_truth(br_map(&prog_local_map) == BR_SUCCESS);

            uint8_t *ptr = (uint8_t *)prog_local_map.vaddr;

            mem_set(ptr, 0, size);
            mem_cpy(ptr, (uint8_t *)elf_header + prog_header->file_offset, prog_header->file_size);

            BrMapArgs prog_map = {
                .space = space,
                .mem_obj = prog_obj.handle,
                .offset = 0,
                .size = 0,
                .vaddr = prog_header->virtual_address,
                .flags = BR_MEM_WRITABLE,
            };

            assert_truth(br_map(&prog_map) == BR_SUCCESS);

            assert_truth(br_unmap(&(BrUnmapArgs){
                             .space = BR_SPACE_SELF,
                             .vaddr = prog_local_map.vaddr,
                             .size = size,
                         }) == BR_SUCCESS);

            assert_truth(brh_close(prog_obj.handle) == BR_SUCCESS);
        }

        prog_header = (Elf64ProgramHeader *)((uint8_t *)prog_header + elf_header->program_header_table_entry_size);
    }
}

BrTask elf_exec(BrMemObj elf_obj, Str name)
{
    BrMapArgs elf_map = {
        .space = BR_SPACE_SELF,
        .mem_obj = elf_obj,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&elf_map) == BR_SUCCESS);

    BrCreateArgs elf_space = {
        .type = BR_OBJECT_SPACE,
        .space = {
            .flags = BR_SPACE_NONE,
        },
    };

    assert_truth(br_create(&elf_space) == BR_SUCCESS);

    BrCreateArgs elf_task = {
        .type = BR_OBJECT_TASK,
        .task = {
            .name = str_cast_fix(StrFix128, name),
            .space = elf_space.handle,
            .caps = BR_CAP_ALL,
        },
    };

    assert_truth(br_create(&elf_task) == BR_SUCCESS);

    Elf64Header *elf_header = (Elf64Header *)elf_map.vaddr;

    elf_load(elf_space.handle, elf_header, elf_obj);

    BrCreateArgs stack_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = 0x4000,
            .flags = BR_MEM_OBJ_NONE,
        },
    };

    assert_truth(br_create(&stack_obj) == BR_SUCCESS);

    BrMapArgs stack_map = {
        .space = elf_space.handle,
        .mem_obj = stack_obj.handle,
        .vaddr = 0xC0000000 - 0x4000,
        .flags = BR_MEM_WRITABLE,
    };

    assert_truth(br_map(&stack_map) == BR_SUCCESS);

    assert_truth(br_start(&(BrStartArgs){
                     .task = elf_task.handle,
                     .ip = elf_header->entry,
                     .sp = 0xC0000000,
                 }) == BR_SUCCESS);

    assert_truth(br_unmap(&(BrUnmapArgs){
                     .space = BR_SPACE_SELF,
                     .vaddr = elf_map.vaddr,
                     .size = elf_map.size,
                 }) == BR_SUCCESS);

    log("Service '{}' created!", name);

    return elf_task.handle;
}
