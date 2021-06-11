#pragma once

#include <brutal/base.h>
#include <brutal/base/error.h>
#include "syscalls/error.h"

typedef result_t(br_error_t, void *) syscall_initialize_result_t;

syscall_initialize_result_t syscall_initialize_for_current_cpu(void);
