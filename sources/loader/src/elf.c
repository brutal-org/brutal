#include "elf.h"
#include <efi/utils.h>
#include "efi/file.h"

ElfResult load_elf_file(char16 *path)
{
    File f = open_file(path);
    Elf64Program ret;

    get_file_info(&f);

    if (f.status != 0)
    {
        close_file(&f);
        return ERR(ElfResult, FILE_NOT_FOUND);
    }

    u8 *file_buf = NULL;

    read_file(&f, file_buf);

    Elf64Header *ehdr = (Elf64Header *)file_buf;

    ret.hdr = *ehdr;

    if (!elf_validate(ehdr))
    {
        return ERR(ElfResult, NOT_AN_ELF_FILE);
    }

    if (ehdr->machine_type != ELF_X86_64)
    {
        return ERR(ElfResult, UNSUPPORTED_MACHINE);
    }

    auto phdr = &ehdr + ehdr->program_header_table_file_offset;

    Elf64ProgramHeader *phdr_ptr;

    for (auto i = 0; i < ehdr->program_header_table_entry_count; i++)
    {
        phdr_ptr = (Elf64ProgramHeader *)phdr;

        if (phdr_ptr->type == ELF_TYPE_EXECUTABLE)
        {
            auto file_segment = (void *)((u64)file_buf + phdr_ptr->file_offset);
            auto mem_segment = (void *)phdr_ptr->virtual_address;

            memcpy(mem_segment, file_segment, phdr_ptr->file_size);

            auto extra_zeroes = (u8 *)mem_segment + phdr_ptr->file_size;
            u64 extra_zeroes_count = phdr_ptr->memory_size - phdr_ptr->file_size;
	    
            if (extra_zeroes_count > 0)
            {
                memset(extra_zeroes, 0x00, extra_zeroes_count);
            }
        }

        phdr_ptr += ehdr->program_header_table_entry_size;
    }

    ret.entry_point = ehdr->entry;

    close_file(&f);

    return OK(ElfResult, ret);
}

char *elf_get_error_message(ElfResult res)
{

    if (!res.succ)
    {
        switch (res.err)
        {
        case NOT_AN_ELF_FILE:
        {
            return "not an elf file";
        }
        case UNSUPPORTED_MACHINE:
        {
            return "unsupported elf architecture";
            break;
        }

        case FILE_NOT_FOUND:
        {
            return "elf file not found";
            break;
        }
        }
    }

    return "this shouldn't happen";
}
