#pragma once

#include <brutal-base>

void syscall_initialize(void);

void syscall_set_gs(uintptr_t addr);
