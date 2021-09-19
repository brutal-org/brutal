#pragma once 
#include <stdint.h>
#if defined(__x86_64__)

static inline void asm_pause(void)
{
    asm volatile ("pause");
}

static inline uint64_t asm_syscall(uint64_t r0, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, uint64_t r5)
{
    uint64_t res; 

    register uint64_t r8 asm("r8") = r4;
    register uint64_t r9 asm("r9") = r5;

    asm volatile("syscall"
                 : "=a"(res)
                 : "a"(r0), "b"(r1), "d"(r2), "S"(r3), "r"(r8), "r"(r9)
                 : "memory", "r11", "rcx");

    return res;
}
#elif defined(__riscv)

static inline void asm_pause(void)
{
}

static inline uint64_t asm_syscall([[maybe_unused]] uint64_t r0,[[maybe_unused]] uint64_t r1,[[maybe_unused]] uint64_t r2,[[maybe_unused]] uint64_t r3, [[maybe_unused]]uint64_t r4, [[maybe_unused]]uint64_t r5)
{
    return 0;
}
#else

static inline void asm_pause(void)
{
}

static inline uint64_t asm_syscall(uint64_t r0, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, uint64_t r5)
{
    return 0;
}
#endif
