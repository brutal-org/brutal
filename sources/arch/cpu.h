#pragma once
#include <stddef.h>
#include "kernel/cpu.h"

size_t arch_cpu_count();

cpuid_t arch_cpu_current_id();

struct cpu* arch_cpu_this();

struct cpu *arch_cpu(cpuid_t id);