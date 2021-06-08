#pragma once

#include <library/base/std.h>

typedef uint8_t cpu_id_t;

struct cpu
{
    cpu_id_t id;
    bool present;
};

void cpu_found(int id);
