#include <elf/elf.h>

bool elf_validate(Elf64Header const *header)
{
    return header->ident.magics[0] == '\177' &&
           header->ident.magics[1] == 'E' &&
           header->ident.magics[2] == 'L' &&
           header->ident.magics[3] == 'F';
}
