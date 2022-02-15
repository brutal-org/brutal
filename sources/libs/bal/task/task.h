#pragma once

#include <bal/hw/mem.h>
#include <ipc/capability.h>

typedef struct
{
    BrId id;
    BrHandle handle;
    Str name;
} BalTask;

void bal_task_init(BalTask *task, Str name);

void bal_task_deinit(BalTask *task);

BrResult bal_TaskExec(BalTask *task, BalMem *elf, BrRight rights, IpcCap *caps, size_t count);
