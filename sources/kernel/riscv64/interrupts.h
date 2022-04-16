#pragma once

#include <brutal-base>

typedef struct
{
    uint64_t r0;
    uint64_t ra;
    uint64_t sp;
    uint64_t gp;
    uint64_t tp;
    uint64_t t[3];
    uint64_t s[2];
    uint64_t a[8];
    uint64_t s2[10];
    uint64_t t2[4];
} Regs;

void init_interrupts(void);
