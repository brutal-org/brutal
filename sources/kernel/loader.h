#pragma once

#include <brutal/base.h>
#include <kernel/tasking.h>
#include <elf/elf.h>
#include <stdint.h>

// the task is not started [!]
task_return_Result program_load(Str name, void *data, size_t size, uintptr_t *start_addr);
