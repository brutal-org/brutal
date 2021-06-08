#include "arch/x86_64/asm.h"

void pit_initialize(int frequency)
{
    uint16_t divisor = 1193182 / frequency;

    asm_out8(0x43, 0x36);
    asm_out8(0x40, divisor & 0xFF);
    asm_out8(0x40, (divisor >> 8) & 0xFF);
}
