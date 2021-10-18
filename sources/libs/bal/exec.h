#pragma once

#include <bal/types.h>
#include <handover/handover.h>

#define BR_MAX_ARGC (256)

typedef struct
{
    int argc;
    Str *argv;
} BrCMainArgs;

typedef struct
{
    BrStartType type;

    union
    {
        struct
        {
            uintptr_t arg1;
            uintptr_t arg2;
            uintptr_t arg3;
            uintptr_t arg4;
            uintptr_t arg5;
        } args;

        BrCMainArgs cmain;
        Handover const *handover;
    };
} BrExecArgs;

typedef struct
{
    BrId tid;
    BrTask handle;
} BrTaskInfos;

BrResult bal_exec(BrMemObj elf_obj, Str name, BrExecArgs const *args, BrTaskInfos *infos);
