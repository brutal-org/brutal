#pragma once

#include <bal/syscalls.h>
#include <elf/elf.h>

void elf_load(BrSpace space, Elf64Header const *elf_header, BrMemObj elf_obj);
