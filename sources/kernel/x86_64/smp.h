#pragma once

enum smp_cpu_init_addr
{
    SMP_INIT_PAGE_TABLE = 0x500,
    SMP_INIT_ENTRY = 0x510,
    SMP_INIT_STACK = 0x520,
    SMP_INIT_GDT = 0x530,
    SMP_INIT_IDT = 0x540,
};

void smp_stop_all(void);

void smp_boot_other(void);
