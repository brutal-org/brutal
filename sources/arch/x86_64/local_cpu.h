#pragma once
#include <stddef.h>

#include "arch/x86_64/apic.h"

#define MAX_CPU_COUNT 255

struct local_cpu
{
    void *syscall_stack_save;
    void *syscall_stack;

    size_t lapic_id;
    size_t cpu_id;
};

struct local_cpu *get_current_cpu_structure();

struct local_cpu *get_cpu_structure(uint8_t cpu_id);