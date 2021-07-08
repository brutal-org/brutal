#include <elf/elf.h>

bool has_valid_elf_header(const struct elf64_header *header)
{
    if (header->identifier.magics[0] != '\177' || header->identifier.magics[1] != 'E' || header->identifier.magics[2] != 'L' || header->identifier.magics[3] != 'F')
    {
        return false;
    }
    return true;
}