#pragma once

#include <brutal-base>

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

enum cr0_bit
{
    CR0_PROTECTED_MODE_ENABLE = (1 << 0),
    CR0_MONITOR_CO_PROCESSOR = (1 << 1),
    CR0_EMULATION = (1 << 2),
    CR0_TASK_SWITCHED = (1 << 3),
    CR0_EXTENSION_TYPE = (1 << 4),
    CR0_NUMERIC_ERROR_ENABLE = (1 << 5),
    CR0_WRITE_PROTECT_ENABLE = (1 << 16),
    CR0_ALIGNMENT_MASK = (1 << 18),
    CR0_NOT_WRITE_THROUGH_ENABLE = (1 << 29),
    CR0_CACHE_DISABLE = (1 << 30),
    CR0_PAGING_ENABLE = (1 << 31),
};

enum cr4_bit
{
    CR4_VIRTUAL_8086_MODE_EXT = (1 << 0),
    CR4_PROTECTED_MODE_VIRTUAL_INT = (1 << 1),
    CR4_TIME_STAMP_DISABLE = (1 << 2), // disable it only for ring != 0 for RDTSC instruction
    CR4_DEBUGGING_EXT = (1 << 3),      // enable debug register break on io space
    CR4_PAGE_SIZE_EXT = (1 << 4),
    CR4_PHYSICAL_ADDRESS_EXT = (1 << 5),
    CR4_MACHINE_CHECK_EXCEPTION = (1 << 6),
    CR4_PAGE_GLOBAL_ENABLE = (1 << 7),
    CR4_PERFORMANCE_COUNTER_ENABLE = (1 << 8),
    CR4_FXSR_ENABLE = (1 << 9),
    CR4_SIMD_EXCEPTION_SUPPORT = (1 << 10), // Operating System Support for Unmasked SIMD Floating-Point Exceptions
    CR4_USER_MODE_INSTRUCTION_PREVENTION = (1 << 11),
    CR4_5_LEVEL_PAGING_ENABLE = (1 << 12),
    CR4_VIRTUAL_MACHINE_EXT_ENABLE = (1 << 13),
    CR4_SAFER_MODE_EXT_ENABLE = (1 << 14),
    CR4_FS_GS_BASE_ENABLE = (1 << 16),
    CR4_PCID_ENABLE = (1 << 17),
    CR4_XSAVE_ENABLE = (1 << 18),
    CR4_SUPERVISOR_EXE_PROTECTION_ENABLE = (1 << 20),
    CR4_SUPERVISOR_ACCESS_PROTECTION_ENABLE = (1 << 21),
    CR4_KEY_PROTECTION_ENABLE = (1 << 22),
    CR4_CONTROL_FLOW_ENABLE = (1 << 23),
    CR4_SUPERVISOR_KEY_PROTECTION_ENABLE = (1 << 24),
};

ASM_MAKE_CRN(0);
ASM_MAKE_CRN(1);
ASM_MAKE_CRN(2);
ASM_MAKE_CRN(3);
ASM_MAKE_CRN(4);

enum xcr0_bit
{
    XCR0_XSAVE_SAVE_X87 = (1 << 0),
    XCR0_XSAVE_SAVE_SSE = (1 << 1),
    XCR0_AVX_ENABLE = (1 << 2),
    XCR0_BNDREG_ENABLE = (1 << 3),
    XCR0_BNDCSR_ENABLE = (1 << 4),
    XCR0_AVX512_ENABLE = (1 << 5),
    XCR0_ZMM0_15_ENABLE = (1 << 6),
    XCR0_ZMM16_32_ENABLE = (1 << 7),
    XCR0_PKRU_ENABLE = (1 << 9),
};

static inline uint64_t asm_read_xcr(uint32_t i)
{
    uint32_t eax, edx;
    asm volatile("xgetbv"

                 : "=a"(eax), "=d"(edx)
                 : "c"(i)
                 : "memory");

    return eax | ((uint64_t)edx << 32);
}

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

static inline void asm_xsave(uint8_t *region)
{
    asm volatile("xsave %0" ::"m"(*region), "a"(~(uintptr_t)0), "d"(~(uintptr_t)0)
                 : "memory");
}

static inline void asm_xrstor(uint8_t *region)
{
    asm volatile("xrstor %0" ::"m"(*region), "a"(~(uintptr_t)0), "d"(~(uintptr_t)0)
                 : "memory");
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
    RFLAGS_ALIGNEMENT_CHECK = 1 << 18, // check each memory alignment
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

static inline void asm_invlpg(uintptr_t addr)
{
    asm volatile("invlpg (%0)" ::"r"(addr)
                 : "memory");
}
