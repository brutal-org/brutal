#pragma once
#include <efi/base.h>
#include <efi/utils.h>
#include "efi/file.h"

#define PT_LOAD 1
#define ELF_X86_64 0x3e

typedef struct
{
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELF64Header;

typedef struct
{
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} ELF64ProgramHeader;

#define EI_MAG0 0 /* Magic number, byte 0. */
#define EI_MAG1 1 /* Magic number, byte 1. */
#define EI_MAG2 2 /* Magic number, byte 2. */
#define EI_MAG3 3 /* Magic number, byte 3. */
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define IS_ELF(ehdr) ((ehdr).e_ident[EI_MAG0] == ELFMAG0 && \
                      (ehdr).e_ident[EI_MAG1] == ELFMAG1 && \
                      (ehdr).e_ident[EI_MAG2] == ELFMAG2 && \
                      (ehdr).e_ident[EI_MAG3] == ELFMAG3)

typedef enum
{
    OK,
    NOT_AN_ELF_FILE,
    UNSUPPORTED_MACHINE,
    FILE_NOT_FOUND
} ELFStatus;

typedef struct
{
    ELF64ProgramHeader phdr;
    ELF64Header hdr;
    ELFStatus status;
    EFIStatus efi_status;
} ELF64Program;

ELF64Program load_elf_file(char16 *path);
