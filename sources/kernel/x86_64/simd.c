#include <brutal/log.h>
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/cpuid.h"
#include "kernel/x86_64/simd.h"

void simd_initialize_fpu(void)
{
    asm_write_cr0(asm_read_cr0() & ~CR0_EMULATION);
    asm_write_cr0(asm_read_cr0() | CR0_NUMERIC_ERROR_ENABLE);
    asm_write_cr0(asm_read_cr0() | CR0_MONITOR_CO_PROCESSOR);

    asm_write_cr4(asm_read_cr4() | CR4_FXSR_ENABLE);
    asm_write_cr4(asm_read_cr4() | CR4_SIMD_EXCEPTION_SUPPORT);

    asm_fninit();
}

void simd_enable_xsave(void)
{
    asm_write_cr4(asm_read_cr4() | CR4_XSAVE_ENABLE);
}

void simd_initialize_xcr0(void)
{
    uint64_t xcr0 = 0;

    xcr0 |= XCR0_XSAVE_SAVE_X87;
    xcr0 |= XCR0_XSAVE_SAVE_SSE;

    if (cpuid_has_avx())
    {
        xcr0 |= XCR0_AVX_ENABLE;
    }

    if (cpuid_has_avx512())
    {
        xcr0 |= XCR0_AVX512_ENABLE;
        xcr0 |= XCR0_ZMM0_15_ENABLE;
        xcr0 |= XCR0_ZMM16_32_ENABLE;
    }

    asm_write_xcr(0, xcr0);
}

void simd_initialize(void)
{
    simd_initialize_fpu();

    if (cpuid_has_xsave())
    {
        log$("XSAVE detected");

        simd_enable_xsave();
        simd_initialize_xcr0();
    }

    log$("SIMD save buffer size = {}", simd_context_size());
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
