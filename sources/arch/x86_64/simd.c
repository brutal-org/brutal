#include <brutal/log.h>
#include "arch/x86_64/asm.h"
#include "arch/x86_64/cpuid.h"
#include "arch/x86_64/simd.h"

void simd_initialize_fpu(void)
{
    asm_write_cr0(asm_read_cr0() & ~ASM_CR0_EMULATION);
    asm_write_cr0(asm_read_cr0() | ASM_CR0_NATIVE_EXCEPTION);
    asm_write_cr0(asm_read_cr0() | ASM_CR0_MONITOR_CO_PROCESSOR);

    asm_write_cr4(asm_read_cr4() | ASM_CR4_FXSR);
    asm_write_cr4(asm_read_cr4() | ASM_CR4_UNMASKED_FPU_EXCEPTION);

    asm_fninit();
}

void simd_enable_xsave(void)
{
    asm_write_cr4(asm_read_cr4() | ASM_CR4_XSAVE);
}

void simd_initialize_xcr0(void)
{
    uint64_t xcr0 = 0;

    xcr0 |= ASM_XCR0_XSAVE_SAVE_X87;
    xcr0 |= ASM_XCR0_XSAVE_SAVE_SSE;

    if (cpuid_has_avx())
    {
        xcr0 |= ASM_XCR0_ENABLE_AVX;
    }

    if (cpuid_has_avx512())
    {
        xcr0 |= ASM_XCR0_ENABLE_AVX512;
        xcr0 |= ASM_XCR0_ENABLE_ZMM0_15;
        xcr0 |= ASM_XCR0_ENABLE_ZMM16_32;
    }

    asm_write_xcr(0, xcr0);
}

void simd_initialize(void)
{
    simd_initialize_fpu();

    if (cpuid_has_xsave())
    {
        log("XSAVE detected");

        simd_enable_xsave();
        simd_initialize_xcr0();
    }

    log("SIMD save buffer size = {}", simd_context_size());
}

size_t simd_context_size(void)
{
    if (cpuid_has_xsave())
    {
        return cpuid_xsave_size();
    }
    else
    {
        return 512;
    }
}

void simd_context_init(void *ptr)
{
    simd_context_save(ptr);
}

void simd_context_save(void *ptr)
{
    if (cpuid_has_xsave())
    {
        asm_xsave(ptr);
    }
    else
    {
        asm_fxsave(ptr);
    }
}

void simd_context_load(void *ptr)
{
    if (cpuid_has_xsave())
    {
        asm_xrstor(ptr);
    }
    else
    {
        asm_fxrstor(ptr);
    }
}
