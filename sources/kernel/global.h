#pragma once

#include <bal/syscalls.h>
#include "kernel/heap.h"
#include "kernel/memory.h"

BrGlobalInfo *global(void);

HeapRange global_region(void);

MemObj *global_create_obj(void);
