#pragma once

#include <brutal/base.h>
#include <elf/elf.h>
#include <kernel/tasking.h>

// the task is not started [!]
TaskCreateResult program_load(Str name, void *data, size_t size, uintptr_t *start_addr);
