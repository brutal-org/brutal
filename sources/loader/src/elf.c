#include "elf.h"
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

    Elf64Header ehdr;

    read_file(&f, &ehdr);

    ret.hdr = ehdr;

    if (!elf_validate(&ehdr))
    {
        return ERR(ElfResult, NOT_AN_ELF_FILE);
    }

    if (ehdr.machine_type != ELF_X86_64)
    {
        return ERR(ElfResult, UNSUPPORTED_MACHINE);
    }

    close_file(&f);

    return OK(ElfResult, ret);
}

char *elf_get_error_message(ElfResult res)
{

    if (!res.success)
    {
        switch (res._error)
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
