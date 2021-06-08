#pragma once
#include <library/base/std.h>

#include "kernel/cpu.h"

size_t arch_cpu_count(void);

cpu_id_t arch_cpu_current_id(void);

struct cpu *arch_cpu_this(void);

struct cpu *arch_cpu(cpu_id_t id);