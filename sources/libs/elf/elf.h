#pragma once

#include <brutal-base>

#define ELF_HEADER_MAGIC "\177ELF"

enum elf_class
{
    ELF_CLASS_INVALID = 0,
    ELF_CLASS_32 = 1,
    ELF_CLASS_64 = 2,
};

enum elf_data_encoding
{
    ELF_ENCODING_INVALID = 0,
    ELF_ENCODING_LITTLE_ENDIAN = 1,
    ELF_ENCODING_BIG_ENDIAN = 2,
};

enum elf_type
{
    ELF_TYPE_NONE = 0,
    ELF_TYPE_RELOCATABLE = 1,
    ELF_TYPE_EXECUTABLE = 2,
    ELF_TYPE_DYNAMIC = 3,
    ELF_TYPE_CORE = 4,
};

enum elf_program_header_types
{
    ELF_PROGRAM_HEADER_NULL = 0,
    ELF_PROGRAM_HEADER_LOAD = 1,
    ELF_PROGRAM_HEADER_DYNAMIC = 2,
    ELF_PROGRAM_HEADER_INTERPRET = 3,
    ELF_PROGRAM_HEADER_NOTE = 4,
};

enum elf_program_header_flags
{
    ELF_PROGRAM_HEADER_EXECUTABLE = 1 << 0,
    ELF_PROGRAM_HEADER_WRITABLE = 1 << 1,
    ELF_PROGRAM_HEADER_READABLE = 1 << 2,
};

typedef struct PACKED
{
    uint8_t magics[4];
    uint8_t elf_class;
    uint8_t data_encoding;
    uint8_t version;
    uint8_t os;
    uint8_t abi_version;
    uint8_t _padding[7];
} Elf64Ident;

static_assert(sizeof(Elf64Ident) == 16, "");

typedef struct PACKED
{
    Elf64Ident ident;

    uint16_t object_type;
    uint16_t machine_type;
    uint32_t object_version;

    uint64_t entry;

    uint64_t programs_offset;
    uint64_t sections_offset;

    uint32_t flags;

    uint16_t size;

    uint16_t programs_size;
    uint16_t programs_count;

    uint16_t sections_size;
    uint16_t sections_count;

    uint16_t strings_section_index;
} Elf64Header;

typedef struct PACKED
{
    uint32_t type;  // elf_program_header_types
    uint32_t flags; // elf_program_header_flags

    uint64_t file_offset;
    uint64_t virtual_address;
    uint64_t physical_address;

    uint64_t file_size;
    uint64_t memory_size;

    uint64_t alignment;
} Elf64ProgramHeader;

typedef struct PACKED
{
    uint32_t name;
    uint32_t type;
    uint64_t flags;

    uint64_t virtual_address;
    uint64_t file_offset;
    uint64_t file_size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entry_size;
} Elf64SectionHeader;

bool elf_validate(Elf64Header const *header);

Str elf_string(Elf64Header const *header, size_t offset);

Str elf_manifest(Elf64Header const *header);

Elf64SectionHeader const *elf_section_by_index(Elf64Header const *header, size_t index);

Elf64SectionHeader const *elf_section_by_name(Elf64Header const *header, Str name);
