#pragma once

#include <bal/hw/mem.h>
#include <bal/task/args.h>

typedef struct
{
    BrId id;
    BrHandle handle;
    Str name;
} BalTask;

void bal_task_init(BalTask *task, Str name);

void bal_task_deinit(BalTask *task);

BrResult bal_task_exec(BalTask *task, BalMem *elf, BrRight rights, BalArgs args);
