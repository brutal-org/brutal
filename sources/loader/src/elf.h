#pragma once
#include <efi/base.h>
#include <efi/utils.h>
#include "efi/file.h"
#include <elf/elf.h>
#include <brutal/base/result.h>

#define ELF_X86_64 0x3E

typedef enum
{
    NOT_AN_ELF_FILE,
    UNSUPPORTED_MACHINE,
    FILE_NOT_FOUND
} ElfStatus;

typedef struct
{
    Elf64ProgramHeader phdr;
    Elf64Header hdr;
} Elf64Program;

typedef Result(ElfStatus, Elf64Program) ElfResult;

ElfResult load_elf_file(char16 *path);

char* elf_get_error_message(ElfResult res);
