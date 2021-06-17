#include "pit.h"
#include "arch/x86_64/asm.h"

uint32_t pit_read_counter(void)
{
    asm_out8(PIT_MODE_COMMAND, 0);
    uint32_t counter = asm_in8(PIT_DATA_PORT0);
    counter |= asm_in8(PIT_DATA_PORT0) << 8;

    return counter;
}

void pit_initialize(int freq_divisor)
{
    uint16_t divisor = PIT_FREQUENCY / freq_divisor;

    asm_out8(PIT_MODE_COMMAND, PIT_CHANNEL1 | PIT_LOWBYTE | PIT_SQUARE_WAVE);
    asm_out8(PIT_DATA_PORT0, divisor & 0xFF);
    asm_out8(PIT_DATA_PORT0, (divisor >> 8) & 0xFF);
}

void pit_sleep(uint16_t ms)
{
    uint16_t wait_val = (ms);

    asm_out8(PIT_MODE_COMMAND, PIT_CHANNEL1 | PIT_LOWBYTE);
    asm_out8(PIT_DATA_PORT0, wait_val & 0xFF);
    asm_out8(PIT_DATA_PORT0, (wait_val >> 8) & 0xFF);

    WAIT_FOR(pit_read_counter() == 0);
}
