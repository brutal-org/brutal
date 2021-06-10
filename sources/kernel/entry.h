#pragma once

#include "kernel/handover.h"

void kernel_entry_main(struct handover *handover);

void kernel_entry_other(void);
