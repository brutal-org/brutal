#include <library/log.h>

#include "arch/x86_64/cpuid.h"

cpuid_result_t cpuid(uint32_t leaf, uint32_t subleaf)
{

    uint32_t cpuid_max;
    asm volatile("cpuid"
                 : "=a"(cpuid_max)
                 : "a"(leaf & 0x80000000)
                 : "rbx", "rcx", "rdx");

    if (leaf > cpuid_max)
    {
        log("CPUID failled leaf:{} subleaf:{}", leaf, subleaf);
        return (cpuid_result_t){.success = false};
    }

    cpuid_result_t result;
    result.success = true;

    asm volatile("cpuid"
                 : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
                 : "a"(leaf), "c"(subleaf));

    return result;
}