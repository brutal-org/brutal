#include "elf.h"
#include "efi/file.h"

ELF64Program load_elf_file(char16 *path)
{
    File f = open_file(path);
    ELF64Program ret;

    ret.efi_status = 0;
    ret.status = OK;

    get_file_info(&f);

    if (f.status != 0)
    {
        close_file(&f);
        ret.efi_status = f.status;
        return ret;
    }

    ELF64Header ehdr;

    read_file(&f, &ehdr);

    ret.hdr = ehdr;

    if (!IS_ELF(ehdr))
    {
        ret.status = NOT_AN_ELF_FILE;
        return ret;
    }

    if (ehdr.e_machine != ELF_X86_64)
    {
        ret.status = UNSUPPORTED_MACHINE;
        return ret;
    }

    close_file(&f);

    return ret;
}
