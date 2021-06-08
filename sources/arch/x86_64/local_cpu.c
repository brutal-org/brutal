#include "arch/x86_64/local_cpu.h"

static struct local_cpu cpu_array[MAX_CPU_COUNT];

struct local_cpu *get_current_cpu_structure()
{
    return &cpu_array[apic_get_current_cpu_id()];
}

struct local_cpu *get_cpu_structure(uint8_t cpu_id)
{

    return &cpu_array[cpu_id];
}