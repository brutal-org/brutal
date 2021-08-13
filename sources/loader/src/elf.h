#pragma once
#include <brutal/base/result.h>
#include <efi/base.h>
#include <efi/utils.h>
#include <elf/elf.h>
#include "efi/file.h"
#include <brutal/base/error.h>

#define ELF_X86_64 0x3E

typedef struct
{
    Elf64ProgramHeader phdr;
    Elf64Header hdr;
    uint64_t entry_point;
} Elf64Program;

typedef Result(Error, Elf64Program) ElfResult;

ElfResult load_elf_file(char16 *path);

char *elf_get_error_message(ElfResult res);
