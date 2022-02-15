#pragma once

#include <bal/abi.h>

#ifdef __x86_64__
#    include "kernel/x86_64/context.h"
#elif defined(__riscv)
#    include "kernel/riscv64/context.h"
#else
#    error "Arch not supported"
#endif

typedef struct _Context Context;

Context *context_create(void);

void context_destroy(Context *self);

void context_start(Context *self, uintptr_t ip, uintptr_t sp, uintptr_t ksp, BrTaskArgs args, BrTaskFlags flags);
