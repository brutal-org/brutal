#pragma once

enum smp_cpu_init_addr
{
    SMP_INIT_PAGE_TABLE = 0x500,
    SMP_INIT_ENTRY = 0x520,
    SMP_INIT_STACK = 0x570,
    SMP_INIT_GDT = 0x580,
    SMP_INIT_IDT = 0x590,
};

void smp_initialize(void);