#pragma once

#include <elf/elf.h>
#include "kernel/task.h"

void init_start(Handover *handover);

BrTask init_handle(void);
