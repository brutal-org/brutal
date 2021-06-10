#pragma once

#include <brutal/base.h>

static inline void asm_cli(void) { asm volatile("cli"); }

static inline void asm_sti(void) { asm volatile("sti"); }

static inline void asm_hlt(void) { asm volatile("hlt"); }

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

#define ASM_CR0_NATIVE_EXCEPTION (1 << 5)
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
    asm volatile("xsave (%0)" ::"r"(region));
}

static inline void asm_xrstor(void *region)
{
    asm volatile("xrstor (%0)" ::"r"(region));
}

static inline void asm_fninit(void)
{
    asm volatile("fninit");
}

static inline void asm_fxsave(void *region)
{
    asm volatile("fxsave (%0)" ::"r"(region));
}

static inline void asm_fxrstor(void *region)
{
    asm volatile("fxrstor (%0)" ::"r"(region));
}
