#pragma once

#include <brutal/base.h>

enum msr_registers
{
    MSR_APIC = 0x1B,
    MSR_EFER = 0xC0000080,
    MSR_STAR = 0xC0000081,
    MSR_LSTAR = 0xC0000082,
    MSR_COMPAT_STAR = 0xC0000083,
    MSR_SYSCALL_FLAG_MASK = 0xC0000084,
    MSR_FS_BASE = 0xC0000100,
    MSR_GS_BASE = 0xC0000101,
    MSR_KERN_GS_BASE = 0xc0000102,
};

enum msr_efer_reg
{
    EFER_ENABLE_SYSCALL = 1,
};

enum msr_star_reg
{
    STAR_KCODE_OFFSET = 32,
    STAR_UCODE_OFFSET = 48,
};

enum msr_syscall_flag_reg
{
    SYSCALL_FLAG_TURN_OFF_INTERRUPT = (1 << 9)
};

static inline uint64_t rdmsr(enum msr_registers msr)
{

    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"((uint64_t)msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(enum msr_registers msr, uint64_t value)
{

    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    asm volatile("wrmsr"
                 :
                 : "c"((uint64_t)msr), "a"(low), "d"(high));
}
