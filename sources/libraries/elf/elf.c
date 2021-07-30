#include <elf/elf.h>

bool elf_validate(Elf64Header const *header)
{
    return header->identifier.magics[0] == '\177' &&
           header->identifier.magics[1] == 'E' &&
           header->identifier.magics[2] == 'L' &&
           header->identifier.magics[3] == 'F';
}
