#pragma once

#include <library/base.h>

static inline void cli(void) { asm volatile("cli"); }

static inline void sti(void) { asm volatile("sti"); }

static inline void hlt(void) { asm volatile("hlt"); }

static inline uint8_t in8(uint16_t port)
{
    uint8_t data;
    asm volatile("inb %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline uint16_t in16(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline uint32_t in32(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline void out8(uint16_t port, uint8_t data)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void out16(uint16_t port, uint16_t data)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void out32(uint16_t port, uint32_t data)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline uint64_t CR2(void)
{
    uint64_t value = 0;

    asm volatile("mov %%cr2, %0"
                 : "=r"(value));
    return value;
}

static inline uint64_t CR3(void)
{
    uint64_t value = 0;

    asm volatile("mov %%cr3, %0"
                 : "=r"(value));
    return value;
}
