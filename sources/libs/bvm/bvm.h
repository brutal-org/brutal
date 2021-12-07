#pragma once

#include <bvm/local.h>
#include <bvm/mem.h>

typedef enum
{
    BVM_RES_RUN,
    BVM_RES_HALT,
    BVM_RES_ERROR,
} BvmRes;

BvmRes bvm_eval(BvmLocal *local, BvmMem *mem);
