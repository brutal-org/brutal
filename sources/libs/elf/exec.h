#pragma once

#include <elf/elf.h>
#include <syscalls/syscalls.h>

void elf_load(BrSpace space, Elf64Header const *elf_header, BrMemObj elf_obj);

