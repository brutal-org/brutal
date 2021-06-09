#pragma once

#include <library/io.h>
#include "kernel/handover.h"

struct writer *arch_debug(void);

void arch_entry_main(struct handover *handover);

void arch_entry_other(void);

void arch_boot_other(void);

void arch_resched_other(void);

void arch_idle(void);
