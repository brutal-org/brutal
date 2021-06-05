#pragma once

#include <library/io.h>

#include "kernel/handhover.h"

struct writer *arch_debug(void);

void arch_entry(struct handover *handover);
