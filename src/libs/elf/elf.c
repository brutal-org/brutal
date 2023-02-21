#include <elf/elf.h>

bool elf_validate(Elf64Header const *header)
{
    return header->ident.magics[0] == '\177' &&
           header->ident.magics[1] == 'E' &&
           header->ident.magics[2] == 'L' &&
           header->ident.magics[3] == 'F';
}

Str elf_string(Elf64Header const *header, size_t offset)
{
    Elf64SectionHeader const *strings = elf_section_by_index(header, header->strings_section_index);

    if (!strings)
    {
        return nullstr;
    }

    return str$((char const *)header + strings->file_offset + offset);
}

Str elf_manifest(Elf64Header const *header)
{
    Elf64SectionHeader const *manifest_section = elf_section_by_name(header, str$(".brutal.manifest"));

    if (!manifest_section)
    {
        return nullstr;
    }

    char const *str_base = (char const *)header + manifest_section->file_offset;
    size_t str_len = manifest_section->file_size;
    return str_n$(str_len, str_base);
}

Elf64SectionHeader const *elf_section_by_index(Elf64Header const *header, size_t index)
{
    Elf64SectionHeader *section = (Elf64SectionHeader *)((uint8_t *)header + header->sections_offset);

    for (size_t i = 0; i < header->sections_count; i++)
    {
        if (i == index)
        {
            return section;
        }

        section = (Elf64SectionHeader *)((uint8_t *)section + header->sections_size);
    }

    return nullptr;
}

Elf64SectionHeader const *elf_section_by_name(Elf64Header const *header, Str name)
{
    Elf64SectionHeader *section = (Elf64SectionHeader *)((uint8_t *)header + header->sections_offset);

    for (size_t i = 0; i < header->sections_count; i++)
    {
        Str section_name = elf_string(header, section->name);

        if (str_eq(section_name, name))
        {
            return section;
        }

        section = (Elf64SectionHeader *)((uint8_t *)section + header->sections_size);
    }

    return nullptr;
}
