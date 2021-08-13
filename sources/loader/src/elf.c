#include "elf.h"
#include <efi/utils.h>
#include "efi/file.h"
#include "elf/elf.h"

ElfResult load_elf_file(char16 *path)
{
    File f = open_file(path);
    Elf64Program ret;

    get_file_info(&f);

    if (f.status != 0)
    {
        close_file(&f);

        return ERR(ElfResult, ERR_UNDEFINED);
    }

    void *file_buf = {0};

    read_file(&f, file_buf);

    auto ehdr = (Elf64Header *)file_buf;

    efi_print("Section count: {}", ehdr->program_header_table_entry_count);
    efi_print("Program headers offset: {}", ehdr->program_header_table_file_offset);
    efi_print("Program headers size: {}", ehdr->program_header_table_entry_size);
    efi_print("Entry point: {x}", ehdr->entry);

    ret.hdr = *ehdr;

    if (!elf_validate(ehdr))
    {
        return ERR(ElfResult, make_error(ERR_KIND_INVALID_DATA, "Not an elf file."));
    }

    if (ehdr->machine_type != ELF_X86_64)
    {
        return ERR(ElfResult, make_error(ERR_KIND_UNSUPPORTED, "Unsupported elf architecture."));
    }

    void *__header = (file_buf + ehdr->program_header_table_file_offset);

    for (int i = 0; i < ehdr->program_header_table_entry_count; i++)
    {

        Elf64ProgramHeader *header = (Elf64ProgramHeader *)__header;
	
        if (header->type == ELF_PROGRAM_HEADER_LOAD)
        {
            efi_print("Found segment ({}) that is executable!", i);

            auto file_segment = (void *)((u64)file_buf + header->file_offset);
            auto mem_segment = (void *)header->virtual_address;

            memcpy(mem_segment, file_segment, header->file_size);

            auto extra_zeroes = (u8 *)mem_segment + header->file_size;
            u64 extra_zeroes_count = header->memory_size - header->file_size;

            if (extra_zeroes_count > 0)
            {
                memset(extra_zeroes, 0x00, extra_zeroes_count);
            }
        }

        __header += ehdr->program_header_table_entry_size;
    }

    ret.entry_point = ehdr->entry;

    close_file(&f);

    return OK(ElfResult, ret);
}

char *elf_get_error_message(ElfResult res)
{

    if (!res.succ)
    {
        return res.err.message.buffer;
    }

    return "this shouldn't happen";
}
