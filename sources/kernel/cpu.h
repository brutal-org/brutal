#pragma once

#include <library/base/std.h>

typedef uint8_t cpu_id_t;

struct cpu
{
    cpu_id_t id;
    bool present;
};

void cpu_found(int id);

void cpu_start(int id); // function called by the cpu

size_t cpu_count(void);
