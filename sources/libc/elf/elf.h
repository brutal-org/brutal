#pragma once

#include <brutal/base.h>

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

struct PACKED elf_identifier
{
    uint8_t magics[4];
    uint8_t elf_class;
    uint8_t data_encoding;
    uint8_t version;
    uint8_t os;
    uint8_t abi_version;
    uint8_t _padding;
};

struct PACKED elf64_header
{
    struct elf_identifier identifier;

    uint16_t object_type;
    uint16_t machine_type;
    uint32_t object_version;

    uintptr_t entry;

    uintptr_t program_header_table_file_offset;
    uintptr_t section_header_table_file_offset;

    uint32_t flags;

    uint16_t elf_header_size;

    uint16_t program_header_table_entry_size;
    uint16_t program_header_table_entry_count;

    uint16_t section_header_table_entry_size;
    uint16_t section_header_table_entry_count;

    uint16_t section_header_string_table_idx;
};

struct PACKED elf64_program_header
{
    uint32_t type;  // elf_program_header_types
    uint32_t flags; // elf_program_header_flags

    uint64_t file_offset;
    uint64_t virtual_address;
    uint64_t physical_address;

    uint64_t file_size;
    uint64_t memory_size;

    uint64_t alignment;
};

bool has_valid_elf_header(const struct elf64_header *header);
