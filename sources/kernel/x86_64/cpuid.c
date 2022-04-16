#include "kernel/x86_64/cpuid.h"
#include <brutal-debug>

CpuidResult cpuid(uint32_t leaf, uint32_t subleaf)
{

    uint32_t cpuid_max;
    asm volatile("cpuid"
                 : "=a"(cpuid_max)
                 : "a"(leaf & 0x80000000)
                 : "rbx", "rcx", "rdx");

    if (leaf > cpuid_max)
    {
        log$("CPUID failed leaf:{} subleaf:{}", leaf, subleaf);
        return (CpuidResult){.succ = false};
    }

    CpuidResult result = {};
    result.succ = true;

    asm volatile("cpuid"
                 : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
                 : "a"(leaf), "c"(subleaf));

    return result;
}
