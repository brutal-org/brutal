#pragma once

#include <brutal/base.h>

typedef struct
{
    bool success;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_result_t;

#define CPUID_XSAVE (1 << 26)
#define CPUID_AVX (1 << 28)
#define CPUID_AVX512 (1 << 16)

cpuid_result_t cpuid(uint32_t leaf, uint32_t subleaf);

static inline bool cpuid_has_xsave(void)
{
    return (cpuid(1, 0).ecx & CPUID_XSAVE) == CPUID_XSAVE;
}

static inline bool cpuid_has_avx(void)
{
    return cpuid(1, 0).ecx & CPUID_AVX;
}

static inline bool cpuid_has_avx512(void)
{
    return cpuid(7, 0).ebx & CPUID_AVX512;
}

static inline size_t cpuid_xsave_size(void)
{
    return cpuid(7, 0).ecx;
}
