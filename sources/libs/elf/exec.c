#include <bal/helpers.h>
#include <bal/syscalls.h>
#include <brutal/log.h>
#include <brutal/mem.h>
#include <elf/exec.h>

void elf_load(BrSpace space, Elf64Header const *elf_header, BrMemObj elf_obj)
{
    Elf64ProgramHeader *prog_header = (Elf64ProgramHeader *)((uint8_t *)elf_header + elf_header->program_header_table_file_offset);

    for (size_t i = 0; i < elf_header->program_header_table_entry_count; i++)
    {
        if (prog_header->type != ELF_PROGRAM_HEADER_LOAD)
        {
            log$("Ignoring segment with type={#0x}", prog_header->type);
            continue;
        }

        size_t size = ALIGN_UP(MAX(prog_header->memory_size, prog_header->file_size), MEM_PAGE_SIZE);

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

            assert_br_success(br_map(&prog_map));
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

            assert_br_success(br_create(&prog_obj));

            BrMapArgs prog_local_map = {
                .space = BR_SPACE_SELF,
                .mem_obj = prog_obj.handle,
                .flags = BR_MEM_WRITABLE,
            };

            assert_br_success(br_map(&prog_local_map));

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

            assert_br_success(br_map(&prog_map));

            assert_br_success(br_unmap(&(BrUnmapArgs){
                .space = BR_SPACE_SELF,
                .vaddr = prog_local_map.vaddr,
                .size = size,
            }));

            assert_br_success(brh_close(prog_obj.handle));
        }

        prog_header = (Elf64ProgramHeader *)((uint8_t *)prog_header + elf_header->program_header_table_entry_size);
    }
}
