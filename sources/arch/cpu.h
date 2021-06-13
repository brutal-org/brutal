#pragma once

#include <brutal/base.h>

typedef uint8_t cpu_id_t;

struct cpu
{
    cpu_id_t id;
    bool present;
};

struct cpu *cpu(cpu_id_t id);

size_t cpu_count(void);

struct cpu *cpu_self(void);

cpu_id_t cpu_self_id(void);
