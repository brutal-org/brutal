#pragma once

#include <elf/elf.h>
#include "kernel/task.h"

void init_start(Handover const *handover);

Task *init_get_task(void);
