#pragma once
#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__)

static inline void asm_pause(void)
{
    asm volatile("pause");
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

static inline void asm_mem_set(void *d, uint8_t s, size_t c)
{
    asm volatile(
        "rep stosb"
        : "=D"(d), "=c"(c)
        : "0"(d), "a"(s), "1"(c)
        : "memory");
}

static inline void asm_mem_copy(void *to, void const *from, size_t size)
{
    asm volatile("rep movsb"
                 : "=D"(to),
                   "=S"(from),
                   "=c"(size)
                 : "D"(to),
                   "S"(from),
                   "c"(size)
                 : "memory");
}
#elif defined(__riscv)

static inline void asm_pause(void)
{
}

static inline uint64_t asm_syscall([[maybe_unused]] uint64_t r0, [[maybe_unused]] uint64_t r1, [[maybe_unused]] uint64_t r2, [[maybe_unused]] uint64_t r3, [[maybe_unused]] uint64_t r4, [[maybe_unused]] uint64_t r5)
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

static inline void asm_mem_set(void *s, uint8_t c, size_t n)
{
    uint8_t *dest = (uint8_t *)s;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = c;
    }
}

static inline void asm_mem_copy(void *to, void const *from, size_t size)
{
    uint8_t *dest = (uint8_t *)to;
    uint8_t const *src = (uint8_t const *)from;

    for (size_t i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
}
#endif
