#pragma once

#include <brutal/base.h>

static inline void asm_cli(void) { asm volatile("cli"); }

static inline void asm_sti(void) { asm volatile("sti"); }

static inline void asm_hlt(void) { asm volatile("hlt"); }

static inline void asm_pause(void) { asm volatile("pause"); }

static inline uint8_t asm_in8(uint16_t port)
{
    uint8_t data;
    asm volatile("inb %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline uint16_t asm_in16(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline uint32_t asm_in32(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline void asm_out8(uint16_t port, uint8_t data)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void asm_out16(uint16_t port, uint16_t data)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void asm_out32(uint16_t port, uint32_t data)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(data), "d"(port));
}

/* --- CR Registers --------------------------------------------------------- */

#define ASM_MAKE_CRN(N)                                \
    static inline uint64_t asm_read_cr##N(void)        \
    {                                                  \
        uint64_t value = 0;                            \
        asm volatile("mov %%cr" #N ", %0"              \
                     : "=r"(value));                   \
        return value;                                  \
    }                                                  \
                                                       \
    static inline void asm_write_cr##N(uint64_t value) \
    {                                                  \
        asm volatile("mov %0, %%cr" #N ::"a"(value));  \
    }

#define ASM_CR0_MONITOR_CO_PROCESSOR (1 << 1)
#define ASM_CR0_EMULATION (1 << 2)
#define ASM_CR0_NATIVE_EXCEPTION (1 << 5)

#define ASM_CR4_FXSR (1 << 9)
#define ASM_CR4_UNMASKED_FPU_EXCEPTION (1 << 10)
#define ASM_CR4_XSAVE (1 << 18)

ASM_MAKE_CRN(0);
ASM_MAKE_CRN(1);
ASM_MAKE_CRN(2);
ASM_MAKE_CRN(3);
ASM_MAKE_CRN(4);

#define ASM_XCR0_XSAVE_SAVE_X87 (1 << 0)
#define ASM_XCR0_XSAVE_SAVE_SSE (1 << 1)
#define ASM_XCR0_ENABLE_AVX (1 << 2)
#define ASM_XCR0_ENABLE_AVX512 (1 << 5)
#define ASM_XCR0_ENABLE_ZMM0_15 (1 << 6)
#define ASM_XCR0_ENABLE_ZMM16_32 (1 << 7)

static inline void asm_write_xcr(uint32_t i, uint64_t value)
{
    uint32_t edx = value >> 32;
    uint32_t eax = (uint32_t)value;
    asm volatile("xsetbv"
                 :
                 : "a"(eax), "d"(edx), "c"(i)
                 : "memory");
}

/* --- XSave/FxSave --------------------------------------------------------- */

static inline void asm_xsave(void *region)
{
    asm volatile("xsave (%0)" ::"a"(region));
}

static inline void asm_xrstor(void *region)
{
    asm volatile("xrstor (%0)" ::"a"(region));
}

static inline void asm_fninit(void)
{
    asm volatile("fninit");
}

static inline void asm_fxsave(void *region)
{
    asm volatile("fxsave (%0)" ::"a"(region));
}

static inline void asm_fxrstor(void *region)
{
    asm volatile("fxrstor (%0)" ::"a"(region));
}

enum rflags_bit
{
    RFLAGS_CARRY = 1 << 0,
    RFLAGS_RESERVED1_ONE = 1 << 1, // reserved, must be 1
    RFLAGS_PARITY = 1 << 2,
    RFLAGS_RESERVED2_ZERO = 1 << 3, // reserved, must be 0
    RFLAGS_AUX_CARRY = 1 << 4,
    RFLAGS_RESERVED3_ZERO = 1 << 5, // reserved, must be 0
    RFLAGS_ZERRO = 1 << 6,
    RFLAGS_SIGN = 1 << 7,
    RFLAGS_TRAP = 1 << 8,             // create an interrupt for each instruction (used for debug)
    RFLAGS_INTERRUPT_ENABLE = 1 << 9, // enable interrupt (like sti)
    RFLAGS_DIRECTION = 1 << 10,
    RFLAGS_OVERFLOW = 1 << 11,
    RFLAGS_IO_PRIVILEGE = 3 << 12,
    RFLAGS_NESTED_TASK = 1 << 14,
    RFLAGS_RESERVED4_ZERO = 1 << 15, // reserved, must be 0
    RFLAGS_RESUME = 1 << 16,
    RFLAGS_VIRTUAL_8086 = 1 << 17,     // good ol' time
    RFLAGS_ALIGNEMENT_CHECK = 1 << 18, // check each memory alignement
    RFLAGS_ACCESS_CONTROL = 1 << 18,
    RFLAGS_VIRTUAL_INTERRUPT = 1 << 19,         // same as INTERRUPT_ENABLE for virtual interrupts
    RFLAGS_VIRTUAL_INTERRUPT_PENDING = 1 << 20, //
    RFLAGS_ID = 1 << 21,                        // support CPUID (i think if this bit is 0 you have a serious problem with your cpu)
};

static inline uint64_t asm_read_rflag(void)
{
    uint64_t value = 0;
    asm volatile(
        "pushf\n"
        "pop %0\n"
        : "=r"(value));
    return value;
}

/* --- MSRs ----------------------------------------------------------------- */

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

static inline uint64_t asm_read_msr(enum msr_registers msr)
{

    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"((uint64_t)msr));
    return ((uint64_t)high << 32) | low;
}

static inline void asm_write_msr(enum msr_registers msr, uint64_t value)
{

    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    asm volatile("wrmsr"
                 :
                 : "c"((uint64_t)msr), "a"(low), "d"(high));
}
