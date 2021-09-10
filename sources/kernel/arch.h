#pragma once

#include <brutal/io.h>
#include <handover/handover.h>
#include "kernel/tasking.h"

IoWriter *arch_debug(void);

void arch_entry_main(Handover *handover);

void arch_entry_other(void);

void arch_idle(void);

void arch_stop(void);

void arch_yield(void);

uint32_t arch_in(uint16_t port, int size);

void arch_out(uint16_t port, int size, uint32_t data);
