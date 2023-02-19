#pragma once

#include <brutal-base>

// doc: https://www.amd.com/system/files/TechDocs/25481.pdf

typedef struct
{
    bool succ;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} CpuidResult;

enum cpuid_leaf
{
    CPUID_FEATURE_IDENTIFIER = 1,
    CPUID_EXTENDED_FEATURE_IDENTIFIER = 7,
    CPUID_PROC_EXTENDED_STATE_ENUMERATION = 13
};

enum cpuid_feature_bits
{
    // ECX
    CPUID_SSSE3_SUPPORT = (1 << 9),
    CPUID_SSE41_SUPPORT = (1 << 19),
    CPUID_SSE42_SUPPORT = (1 << 20),
    CPUID_AES_SUPPORT = (1 << 25),
    CPUID_XSAVE_SUPPORT = (1 << 26),
    CPUID_XSAVE_ENABLED = (1 << 27),
    CPUID_AVX_SUPPORT = (1 << 28),
};

enum cpuid_extended_feature_bits
{
    // EBX
    CPUID_BIT_MANIPULATION_SUPPORT = (1 << 3),
    CPUID_AVX512_SUPPORT = (1 << 16),
};

CpuidResult cpuid(uint32_t leaf, uint32_t subleaf);

static inline bool cpuid_has_xsave(void)
{
    return (cpuid(CPUID_FEATURE_IDENTIFIER, 0).ecx & CPUID_XSAVE_SUPPORT) == CPUID_XSAVE_SUPPORT;
}

static inline bool cpuid_has_avx(void)
{
    return cpuid(CPUID_FEATURE_IDENTIFIER, 0).ecx & CPUID_AVX_SUPPORT;
}

static inline bool cpuid_has_avx512(void)
{
    return cpuid(CPUID_EXTENDED_FEATURE_IDENTIFIER, 0).ebx & CPUID_AVX512_SUPPORT;
}

static inline size_t cpuid_xsave_size(void)
{
    return cpuid(CPUID_PROC_EXTENDED_STATE_ENUMERATION, 0).ecx;
}
