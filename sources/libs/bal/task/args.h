#pragma once

#include <bal/abi/types.h>
#include <bal/boot/handover.h>

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
} BalArgs;

static inline BalArgs bal_args5(uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4, uintptr_t arg5)
{
    return (BalArgs){
        .type = BR_START_ARGS,
        .args = {
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
        },
    };
}

static inline BalArgs bal_args4(uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4)
{
    return bal_args5(arg1, arg2, arg3, arg4, 0);
}

static inline BalArgs bal_args3(uintptr_t arg1, uintptr_t arg2, uintptr_t arg3)
{
    return bal_args5(arg1, arg2, arg3, 0, 0);
}

static inline BalArgs bal_args2(uintptr_t arg1, uintptr_t arg2)
{
    return bal_args5(arg1, arg2, 0, 0, 0);
}

static inline BalArgs bal_args1(uintptr_t arg1)
{
    return bal_args5(arg1, 0, 0, 0, 0);
}

static inline BalArgs bal_args_cmain(Str *args)
{
    int count = 0;

    while (args[count].buf)
    {
        count++;
    }

    return (BalArgs){
        .type = BR_START_CMAIN,
        .cmain = {
            count,
            args,
        },
    };
}

static inline BalArgs bal_args_handover(Handover const *ho)
{
    return (BalArgs){
        .type = BR_START_HANDOVER,
        .handover = ho,
    };
}
