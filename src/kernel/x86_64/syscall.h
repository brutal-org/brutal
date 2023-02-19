#pragma once

#include <brutal-base/std.h>

void syscall_initialize(void);

void syscall_set_gs(uintptr_t addr);
