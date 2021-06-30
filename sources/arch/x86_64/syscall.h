#pragma once

#include <brutal/base.h>
#include "syscalls/error.h"

typedef Result(BrError, void *) syscall_initialize_Result;

syscall_initialize_Result syscall_initialize(void);
