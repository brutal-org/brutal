#pragma once

enum smp_cpu_init_addr
{
    SMP_INIT_PAGE_TABLE = 0x500,
    SMP_INIT_START_ADDR = 0x520,
    SMP_INIT_STACK_ADDR = 0x570,
    SMP_INIT_GDT = 0x580,
    SMP_INIT_IDT = 0x590,
};

#define SMP_CPU_STACK_SIZE (8192)

void smp_initialize(void);