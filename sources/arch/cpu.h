#pragma once

#include <brutal/base/std.h>
#include "kernel/cpu.h"

cpu_id_t arch_cpu_current_id(void);

struct cpu *arch_cpu_this(void);

struct cpu *arch_cpu(cpu_id_t id);
